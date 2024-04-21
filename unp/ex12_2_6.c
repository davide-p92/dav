#include "unp.h"

int main(int argc, char **argv) {
	uint8_t ipaddr[16];
	int sockfd;
	struct sockaddr_in6 saddr;
	int reuse = 1;

	if(argc != 2) {
		printf("Usage: %s <IPv6>\n", argv[0]);
		return -1;
	}
	//Conver IPv6 string to bin
	Inet_pton(AF_INET6, argv[1], ipaddr);
	//ipaddr = argv[1];
	//Create IPv6 TCP socket
	sockfd = Socket(AF_INET6, SOCK_STREAM, 0);
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	//Constr saddr
	//with copy of the IPv6 address in sockaddr_in6 structure
	memcpy(saddr.sin6_addr.s6_addr, ipaddr, sizeof(ipaddr));
	saddr.sin6_family = AF_INET6;
	//saddr.sin6_addr.s6_addr = ipaddr;
	saddr.sin6_port = htons(9999);

	//Bind that to port
	Bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
	
	//Listen
	Listen(sockfd, 10);

	//Pause, wait for SIG
	if(pause() == -1)
		printf("Exit: %d - %s\n", errno, strerror(errno));

	return 0;
}
