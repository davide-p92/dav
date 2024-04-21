#include "unp.h"
#include "readline.c"

void str_cli(FILE *fp, int sockfd) {
	char sendline[MAXLINE], recvline[MAXLINE];

	while(fgets(sendline, MAXLINE, fp) != NULL) {
		Writen(sockfd, sendline, strlen(sendline));
		if(readline(sockfd, recvline, MAXLINE) == 0) {
			printf("str_cli: server terminated prematurely\n");
			printf("Error. %s\n", strerror(errno));
		}
		fputs(recvline, stdout);
	}
}

//Multiple spawning of processes (5 conns to serv)
int main(int argc, char **argv) {
	int i, sockfd[5];
	struct sockaddr_in servaddr;

	if(argc != 2) {
		printf("Usage: %s <IPaddr>\n", argv[0]);
		exit(EXIT_FAILURE);
	}
	
	for(i = 0; i < 5; i++) {
		sockfd[i] = Socket(AF_INET, SOCK_STREAM, 0);
		bzero(&servaddr, sizeof(servaddr));
		servaddr.sin_family = AF_INET;
		servaddr.sin_port = htons(SERV_PORT);
		Inet_pton(AF_INET, argv[1], &servaddr.sin_addr);

		Connect(sockfd[i], (struct sockaddr *) &servaddr, sizeof(servaddr));
	}

	str_cli(stdin, sockfd[0]); //Doitall
	exit(0);
}
