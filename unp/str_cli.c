#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];
	int n;
	while(Fgets(sendline, MAXLINE, fp) != NULL) {
		//Writen(sockfd, sendline, strlen(sendline));
		send(sockfd, sendline, strlen(sendline), 0);
		/*
		if(Readline(sockfd, recvline, MAXLINE) == 0) {
			printf("str_cli: server terminated prematurely\n");
			exit(EXIT_FAILURE);
		}*/
		if((n = recv(sockfd, recvline, MAXLINE - 1, 0)) == 0) {
			fprintf(stderr, "Server terminated prematurely\n");
			break;
		}
		//Null-terminate received data
		recvline[n] = '\0';
		Fputs(recvline, stdout);
	}
}
