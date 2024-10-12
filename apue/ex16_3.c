#include "apue.h"
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/select.h>
#include <errno.h>
#include <ioctl.h>

#define BUFLEN 128
#define QLEN 10

#ifndef HOST_NAME_MAX
#define HOST_NAME_MAX 256
#endif

#ifndef SERV_PORT
#define SERV_PORT 4000
#endif

extern int initserver(int, const struct sockaddr *, socklen_t, int);

void serve(int sockfd) {
	int clfd;
	FILE *fp;
	char buf[BUFLEN];

	set_cloexec(sockfd);

	for( ; ; ) {
		if((clfd = accept(sockfd, NULL, NULL)) < 0) {
			syslog(LOG_ERR, "ruptimed2: accept error: %s", strerror(errno));
			exit(1);
		}
		set_cloexec(clfd);
		if((fp = popen("/usr/bin/uptime", "r")) == NULL) {
			sprintf(buf, "error: %s\n", strerror(errno));
			send(clfd, buf, strlen(buf), 0);
		} else {
			while(fgets(buf, BUFLEN, fp) != NULL)
				send(clfd, buf, strlen(buf), 0);
			pclose(fp);
		}
		close(clfd);
	}
}

int main(int argc, char *argv[]) {
	struct addrinfo *ailist, *aip;
	struct addrinfo hint;
	int sockfd, err, n, desc_ready;
	char *host;
	int end_server = FALSE;

	if(argc != 1)
		err_quit("usage: ruptimed2");
	if((n = sysconf(_SC_HOST_NAME_MAX)) < 0)
		n = HOST_NAME_MAX; //best guess
	if((host = malloc(n)) == NULL)
		err_sys("malloc error");
	if(gethostname(host, n) < 0)
		err_sys("gethostname error");
	daemonize("ruptimed2");
	memset(&hint, 0, sizeof(hint));
	hint.ai_flags = AI_CANONNAME;
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;
	fd_set wrfds, new_fd;
	int on = 1;
	struct timeval tout;
	
	sockfd = socket(AF_INET6, SOCK_STREAM, 0);
	if(sockfd < 0)
		err_sys("error creating socket");

	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (char *)&on, sizeof(on)) < 0)
		err_sys("setsockopt error");
	/*set socket to be nonblocking*/
	if(ioctl(sockfd, FIONBIO, (char *)&on) < 0)
		err_sys("ioctl eror");

	memset(&hint, 0, sizeof(hint));
	hint.sin6_family = AF_INET6;
	hint.sin6_port = htons(SERV_PORT);
	
	/*bind socket*/
	if(bind(sockfd, (struct sockaddr *)&hint, sizeof(hint)) < 0)
		err_sys("bind error");

	/*listen*/
	if(listen(sockfd, 32) < 0)
		err_sys("error listen");
	
	//upper limit of fdset entries
	int maxset = sockfd;
	int s, rc, new_sd;

	//zero wr fdset at each iter
	FD_ZERO(&wrfds);
	FD_SET(sockfd, &wrfds);
	/*initialize timeout 3 s*/
	timeout.tv_sec = 3 * 60;
	timeout.tv_usec = 0;

	/*loop waiting for incoming connections*/
	do {
		printf("waiting on select()..\n");
		if((s =select(maxset + 1, &wrfds,sizeof(wrfds))) < 0)
			err_sys("error select");
		if(s == 0) {
			printf("select timed out\n");
			break;
		}
		desc_ready = s;
		for(int i = 0; i <= maxset && desc_ready >0; i++) {
			if(FD_ISSET(i, &wrfds)) {
				//descr found that was readable.
				desc_ready -= 1;
				/*check if it is listening socket*/
				if(i == sockfd) {
					printf("listening socket is readable\n");;
					//accept incoming conns queued
					do {
						new_sd = accept(sockfd, NULL, NULL);
						if(new_sd < 0) {
							if(errno != EWOULDBLOCK) {
								printf("accept() failed");
								end_server = TRUE;
							}
							break;
						}
						//Add incoming conn to master read set
						printf("\tnew incoming conn to master read set- %d\n", new_sd);
						FD_SET(new_sd, &wrfds);
						if(new_sd > maxset)
							maxset = new_sd;
					} while(new_sd != -1);

				}
				/*this is not the listening socket, therefore
				 * an existing conn has to be readable
				 */
				else {
					printf(" descriptor %d is readable\n", i);
					close_conn = FALSE;
					/*receive incoming data on socket*/
					do {
						/*receive data on this conn until recv failse with EWOULDBLOCK*/
						rc = recv(i, buffer, sizeof(buffer), 0);
						if(rc < 0) {
							if(errno != EWOULDBLOCK) {
								printf("recv() failed");
								close_conn = TRUE;
							}
							break;
						}
						/*check to see if conn has been closed by client */
						if(rc == 0) {
							printf("conn closed\n");
							close_conn = TRUE;
							break;
						}
						/*data was recvd*/
						printf(" %d bytes recvd", rc);
						int len = rc;
						/*echo data back to client*/
						if((rc = send(i, buf, len, 0)) < 0) {
							printf("send () error\n");
							close_conn = TRUE;
							break;
						}
					} while(TRUE);
					/*clean up active conns and remove
					 * descr from fdset */
				       if(close_conn) {
						close(i);
				 		FD_CLR(i, &wrfds);
						if(i == maxset) {
							while(FD_ISSET(maxset, &wrfds) == FALSE)
								maxset--;
						}
					}
				} /*end of existing conn is readable
					 */
			} /*end of if(FD_ISSET(i, &wrfds)*/
		} //end of sel descriptors

	} while(end_server == FALSE);		

	/*clean all socks open*/
	for(i = 0; i <= maxset; i++)
	{
		if(FD_ISSET(i, &wrfds)) {
			close(i);
		}
	}
}




