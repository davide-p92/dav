#include "unp.h"

int main(int argc, char **argv) {
	//Initialize pointer of IP address
	char ipaddr[INET_ADDRSTRLEN];
	int sockfd;
	struct sockaddr_in saddr;
	int reuse = 1;

	if(argc != 2) {
		printf("Usage: %s <IPv4>\n", argv[0]);
		return -1;
	}
	//Inet_pton(AF_INET, argv[1], ipaddr);
	//ipaddr = argv[1];
	//Create IPv4 TCP socket
	sockfd = Socket(AF_INET, SOCK_STREAM, 0);
	//Use setsockopt to avoid address already in use error
	setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

	memset(&saddr, 0, sizeof(saddr));
	//Constr saddr
	saddr.sin_family = AF_INET;
	//saddr.sin_addr.s_addr = ipaddr;
	Inet_pton(AF_INET, argv[1], &(saddr.sin_addr));
	saddr.sin_port = htons(9999);

	//Bind that to port
	Bind(sockfd, (struct sockaddr *) &saddr, sizeof(saddr));
	
	//Listen
	Listen(sockfd, 10);

	//Pause, wait for SIG
	if(pause() == -1)
		printf("Exit: %d - %s\n", errno, strerror(errno));

	return 0;
}
