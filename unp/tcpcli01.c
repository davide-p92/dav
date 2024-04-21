#include "unp.h"
//#include "readline.c"

//void str_cli(FILE *fp, int sockfd);

int main(int argc, char **argv) {
	int sockfd;
	struct sockaddr_in servaddr;

	if(argc != 2) {
		printf("Usage: %s <IPaddr>\n", argv[0]);
		return -1;
	}

	sockfd = Socket(AF_INET, SOCK_STREAM, 0);

	bzero(&servaddr, sizeof(servaddr));

	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(SERV_PORT);
	Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

	Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));

	str_cli(stdin, sockfd);

	exit(0);
}
/*
void str_cli(FILE *fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, MAXLINE);

		if(readline(sockfd, recvline, MAXLINE) < 0) {
			printf("Error readline. %s\n", strerror(errno));
			exit(EXIT_FAILURE);
		} else if(readline(sockfd, recvline, MAXLINE) == 0){
			printf("str_cli: server terminated prematurely.");
			printf("\n");
			exit(EXIT_FAILURE);
		}
		if(fputs(recvline, stdout) == EOF) {
			printf("fputs: error %s", strerror(errno));
			exit(EXIT_FAILURE);
		}
	}
}
*/		
