#include "unp.h"
#include <time.h>
#include "unp.c"


int main(int argc, char **argv) {
	int listenfd, connfd;
	socklen_t len;
	struct sockaddr_in servaddr, cliaddr;
	char buff[MAXLINE];
	time_t ticks;

	listenfd = Socket(AF_INET, SOCK_STREAM, 0);
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
	servaddr.sin_port = htons(9999); //Daytime srv
	
	Bind(listenfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	Listen(listenfd, LISTENQ);

	for( ; ; ) {
		len = sizeof(cliaddr);
		connfd = Accept(listenfd, (struct sockaddr *) &cliaddr, &len);
		printf("Connection from %s, port %d\n", inet_ntop(AF_INET, &cliaddr.sin_addr, buff, sizeof(buff)), ntohs(cliaddr.sin_port));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		if(write(connfd, buff, strlen(buff)) < 0) {
			printf("Error write %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}
		if(close(connfd) < 0) {
			printf("Error close. %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		}

	}
}


