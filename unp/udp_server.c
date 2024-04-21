#include "unp.h"

int udp_server(const char *host, const char *serv, socklen_t *addrlenp) {
	int sockfd, n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if((n = getaddrinfo(host, serv, &hints, &res)) != 0) {
		printf("udp_server error for %s, %s: %s", host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if(sockfd < 0)
			continue; /*error - try next one */
		if(bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break; /*success*/
		Close(sockfd);
	} while((res = res->ai_next) != NULL);

	if(res == NULL) {
		printf("udp_server error for %s, %s", host, serv);
		return -1;
	}
	if(addrlenp)
		*addrlenp = res->ai_addrlen; /*return size of protocol*/

	freeaddrinfo(ressave);

	return(sockfd);
}
