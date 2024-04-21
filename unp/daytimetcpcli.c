#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define NTP_PORT 123
#define NTP_SERVER "pool.ntp.org"
#define MAXLINE 1024


int main(int argc, char **argv) {
	//char *hostname;
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;

	if (argc != 2) {
		fprintf(stderr, "Usage: %s <IPaddress>\n");
		exit(EXIT_FAILURE);
	}

	//strcpy(hostname, argv[1]);
	if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
		fprintf(stderr, "socket error");
		exit(EXIT_FAILURE);
	}

	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_port = htons(9999);
	//struct hostent *host;
	//host = gethostbyname(hostname);
	if(inet_pton(AF_INET, argv[1]/*host->h_name*/, &servaddr.sin_addr) <= 0) {
		fprintf(stderr, "inet_pton err for %s", argv[1]);
		exit(EXIT_FAILURE);
	}

	if(connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr)) < 0) {
		fprintf(stderr, "connect err");
		printf("Error: %s", strerror(errno));
		exit(EXIT_FAILURE);
	}

	//call to getsockname
	struct sockaddr_in sockaddr;
	socklen_t socklen;
	if(getsockname(sockfd, (struct sockaddr *) &sockaddr, sizeof(sockaddr)) == -1) {
		fprintf(stderr, "getsockname error");
		printf("Error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	printf("Address of sockaddr: %s\n", sockaddr->sin_addr);
	char[MAXLINE] sockaddr_portn;
	if(inet_ntop(AF_INET, &sockaddr->sin_addr, sockaddr_portn, sizeof(sockaddr_portn)) != NULL) 
		printf("Port of sockaddr: %d\n", sockaddr_portn);
	//counter for ex. 1.4
	int i = 0;
	while((n = read(sockfd, recvline, MAXLINE)) > 0) {
		i++;
		printf("found i = %d", i);
		recvline[n] = 0;
		if(fputs(recvline, stdout) == EOF) {
			fprintf(stderr, "fputs error");
			exit(EXIT_FAILURE);
		}
	}
	if(n < 0) {
		fprintf(stderr, "read err");
		exit(EXIT_FAILURE);
	}
	exit(0);
}
