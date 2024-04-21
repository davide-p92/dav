#include "unp.h"
#include <sys/resource.h>

int main(int argc, char **argv) {
	int i, maxi, maxfd, listenfd, connfd, sockfd;
	int nready, client[FD_SETSIZE];
	//Set number of fds in fd_set to the allowed virtual memory
	int resource = RLIMIT_NOFILE;
	//Struct to set the limit of max open fds
	struct rlimit rlim;
	//Initialize rlim to max allowed fds by kernel
	if((rlim = getrlimit(resource, &rlim)) != 0) {
		printf("Error getrlimit: %s\n", strerror(errno));
		return -1;
	}
	ssize_t n;
	fd_set rset, allset;
	char buf[MAXLINE];
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	printf("rlim_max: %d\n", rlim.rlim_max);
	if(listenfd > rlim.rlim_max) {
		printf("Listenfd is too high. ABORTING\n");
		return -1;
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);

	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	maxfd = listenfd;
	maxi = -1;
	for(i = 0; i < FD_SETSIZE; i++) /*initialize*/
		client[i] = -1; /*-1 indicates available */

	FD_ZERO(&allset);
	FD_SET(listenfd, &allset);

	for( ; ; ) {
		rset = allset; /*structure assignment */
		nready = Select(maxfd + 1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(listenfd, &rset)) { /*new client conn*/
			clilen = sizeof(cliaddr);
			connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
			for(i = 0; i < FD_SETSIZE; i++) {
				if(client[i] < 0) {
					client[i] = connfd; /* save descriptor */
					break;
				}
			}
			if(i == FD_SETSIZE) {
				printf("Too many clients\n");
				return -1;
			}

			FD_SET(connfd, &allset); /*add new descriptor to queue */
			if(connfd > maxfd)
				maxfd = connfd;
			if(i > maxi)
				maxi = i; /*max index in client[] array */

			if(--nready <= 0)
				continue; //no more readable descriptors
		}
		for(i = 0; i <= maxi; i++) { /*check all clients for data*/
			if((sockfd = client[i]) < 0)
				continue;
			if(FD_ISSET(sockfd, &rset)) {
				if((n = Read(sockfd, buf, MAXLINE)) == 0) {
					/*connection closed by client*/
					Close(sockfd);
					FD_CLR(sockfd, &allset);
					client[i] = -1;
				} else
					Writen(sockfd, buf, n);

				if(--nready <= 0)
					break; /*no more readable descriptors*/
			}
		}
	}
}
