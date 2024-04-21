#include "unp.h"

int udp_client(const char *host, const char *serv, struct sockaddr **saptr, socklen_t lenp) {
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		printf("udp_client error for %s, %s: %s", host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd >= 0)
			break;
	} while((res = res->ai_next) != NULL);

	if(res == NULL) {
		printf("udp_client error for %s, %s", host, serv);
		return -1;
	}

	*saptr = malloc(res->ai_addrlen);
	if(saptr == NULL) {
		printf("error malloc. %s\n"; strerror(errno));
		return -1;
	}
	memcpy(*saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);

	return (sockfd);
}