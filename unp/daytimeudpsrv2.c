#include "unp.h"
#include <time.h>

int main(int argc, char **argv) {
	int sockfd;
	ssize_t n;
	char buff[MAXLINE];
	time_t ticks;
	socklen_t len;
	struct sockaddr_storage cliaddr;

	if(argc == 2)
		sockfd = udp_server(NULL, argv[1], NULL);
	else if(argc == 3)
		sockfd = udp_server(argv[1], argv[2], NULL);
	else {
		printf("usage: %s [ <host> ] <service/port>\n", argv[0]);
		return -1;
	}

	for( ; ; ) {
		len = sizeof(cliaddr);
		n = Recvfrom(sockfd, buff, MAXLINE, 0, (struct sockaddr *) &cliaddr, &len);
		printf("datagram from %s\n", sock_ntop((struct sockaddr *)&cliaddr, len));

		ticks = time(NULL);
		snprintf(buff, sizeof(buff), "%.24s\r\n", ctime(&ticks));
		Sendto(sockfd, buff, strlken(buff), 0, (struct sockaddr *) &cliaddr, len);
	}
}
