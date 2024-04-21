#include "unp.h"

int main(int argc, char **argv) {
	int sockfd, n;
	char recvline[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;

	if(argc != 3) {
		printf("usage: %s <hostname/IP> <service/port>\n", argv[0]);
		return -1;
	}

	sockfd = tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	getpeername(sockfd, (struct sockaddr *) &ss, &len);
	printf("connected to %s\n", sock_ntop_host((struct sockaddr *)&ss, len));

	while((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0; /*null-terminate*/
		Fputs(recvline, stdout);
	}
	return 0;
}
