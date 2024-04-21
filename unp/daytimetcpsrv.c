#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <netdb.h>
#include <netinet/in.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>

#define MAXLINE 1024
#define LISTENQ 5


int main(int argc, char **argv) {
	int listenfd, connfd;
	struct sockaddr_in servaddr;
	char buff[MAXLINE];
	time_t ticks;
	//int errno = 0;
	if((listenfd = socket(AF_INET, SOCK_STREAM, 0)) <= 0) {
		fprintf(stderr, "Error socket");
		//exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY); /*daytimeserv*/
	servaddr.sin_port = htons(9999);
	printf("LISTENFD: %d", listenfd);
	if(bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "Error bind.");
		fprintf(stdout, "%s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	if(listen(listenfd, LISTENQ) < 0) {
		fprintf(stderr, "Error listen\n");
		exit(EXIT_FAILURE);
	}
	for( ; ; ) {
		if((connfd = accept(listenfd, (struct sockaddr *) NULL, NULL)) < 0) {
			fprintf(stderr, "Error accept");
			exit(EXIT_FAILURE);
		}

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		for (int i = 0; i < strlen(buff); i++) {
			if(write(connfd, buff[i], strlen(buff[i])) < 0) {
				fprintf(stderr, "Error write");
				exit(EXIT_FAILURE);
			}
		}

		if(close(connfd) < 0) {
			fprintf(stderr, "Error close");
		}

	}
}
