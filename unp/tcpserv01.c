#include "unp.h"
#include "signal.c"
#include <sys/wait.h>


//void str_echo(int sockfd);

int main(int argc, char **argv) {
	int listenfd, connfd;
	pid_t childpid;
	socklen_t clilen;
	struct sockaddr_in cliaddr, servaddr;
	
	listenfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(SERV_PORT);
	socklen_t servlen = (socklen_t)sizeof(servaddr);	
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	//ADDED:signal handler
	//signal(SIGCHLD, sig_chld);	

	for( ; ; ) {
		clilen = (socklen_t)sizeof(cliaddr);
		//connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &clilen);
		if(connfd = accept(listenfd, (struct sockaddr *) &cliaddr, &clilen) < 0) {
			printf("Connfd not entered.");
			if(errno == EINTR)
				continue;
			else {
				printf("Error accept.%s\n", strerror(errno));
				exit(EXIT_SUCCESS);
			}
		}
		//Added: signal handler
		//signal(SIGCHLD,sig_chld);

		if((childpid = Fork()) == 0) {
			printf("Fork succeeded.\n");
			str_echo(connfd);
			Close(listenfd);
			//str_echo(connfd);
			exit(0);
		}
		//wait(childpid);
		Close(connfd);
	}
}
/*
void str_echo(int sockfd) {
	ssize_t n;
	char buf[MAXLINE];
again:
	while((n = read(sockfd, buf, MAXLINE)) > 0) {
		if(writen(sockfd, buf, n) < 0) {
			printf("Error writen. %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
	if(n < 0 && errno == EINTR)
		goto again;
	else if (n < 0)
		fprintf(stderr, "str_echo: read error");
		exit(EXIT_FAILURE);
	}
}*/
