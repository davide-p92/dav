#include "unp.h"

int Getpeername(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	if(getpeername(sockfd, addr, addrlen) < 0) {
		printf("error Getpeername, %s\n", strerror(errno));
		return -1;
	}
}
