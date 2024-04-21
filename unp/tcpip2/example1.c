#include <stdio.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#define BUFFSIZE 151

int main() {
	struct sockaddr_in serv;
       	struct sockaddr_in *ipv4;
	struct addrinfo hints, *res;
	char buff[BUFFSIZE];
	int sockfd, n;

	if((sockfd = socket(PF_INET, SOCK_DGRAM, 0)) < 0){
		perror("socket error");
		return -1;
	}

	bzero((char *) &serv, sizeof(serv));
	//bzero((char *) &ipv4, sizeof(ipv4));
	//initialize memory for addrinfo structure
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_DGRAM;
	int status = getaddrinfo("en.wikipedia.org", NULL, &hints, &res);
	if(status != 0) {
		printf("getaddrinfo error: %s\n", strerror(errno));
		printf("error: %s\n", gai_strerror(status));
		return -1;
	}

	ipv4 = (struct sockaddr_in *)res->ai_addr;

	//print IP addr
	char ip_address[INET_ADDRSTRLEN];
	inet_ntop(AF_INET, &(ipv4->sin_addr), ip_address, INET_ADDRSTRLEN);
	printf("IP address of %s: %s\n", "en.wikipedia.org", ip_address);

	serv.sin_family = AF_INET;
	serv.sin_addr.s_addr = ipv4->sin_addr.s_addr;
	serv.sin_port = htons(13);

	if(sendto(sockfd, buff, BUFFSIZE, 0, (struct sockaddr *)&serv, sizeof(serv)) != BUFFSIZE) {
		perror("sendto error");
		printf("%s\n", strerror(errno));
		return -1;
	}
	
	if((n = recvfrom(sockfd, buff, BUFFSIZE, 0, (struct sockaddr *) NULL, (int *) NULL)) < 2) {
		printf("recvfromerror:%s\n", strerror(errno));
	       return -1;
	}
	buff[n - 2] = 0;
	printf("%s\n", buff);
	exit(0);
}	
