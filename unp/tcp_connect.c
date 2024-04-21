#include "unp.h"

int tcp_connect(const char *host, const char *serv) {
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		printf("tcp_connect error for %s, %s: %s",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue;
		if(connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;
		Close(sockfd); /*ignore this one*/
	} while((res = res->ai_next) != NULL) ;
	if(res == NULL) { /*errno set from final conn */
		printf("tcp_connect error for %s, %s\n", host, serv);
		return -1;
	}
	freeaddrinfo(ressave);
	
	return (sockfd);
}

int Tcp_connect(const char *host, const char *serv) {
	if(tcp_connect(host, serv) < 0) {
		printf("error Tcp_connect\n");
		return -1;
	}
}
