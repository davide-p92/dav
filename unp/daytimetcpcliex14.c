#include "unp.h"
#include <sys/ioctl.h>


int main(int argc, char **argv) {
	int sockfd, n, m;
	char recvline[MAXLINE + 1], recvlinem[MAXLINE + 1];
	socklen_t len;
	struct sockaddr_storage ss;

	if(argc != 3) {
		printf("usage: %s <hostname/IPaddr> <service/port#>\n", argv[0]);
		return -1;
	}

	sockfd = Tcp_connect(argv[1], argv[2]);

	len = sizeof(ss);
	Getpeername(sockfd, (struct sockaddr *) &ss, &len);
	printf("connected to %s\n", Sock_ntop_host((struct sockaddr *) &ss, len));
	for(;;) {
		m = recv(sockfd, recvlinem, MAXLINE, MSG_PEEK);
		if(m < 0) {
			printf("error recv. %s\n", strerror(errno));
			break;
		} else if(m == 0) {
			printf("EOF. shutting down on recv.\n");
			break;
		} else {
			/*success in recv, calling ioctl to read immediate available bytes in recvlinem buffer */
			int k;
			if((k = ioctl(sockfd, FIONREAD, &k)) < 0) {
				printf("Error ioctl: %s\n", strerror(errno));
				break;
			}
		       	printf("Available bytes in socket queue: %d\nread from recv: %d", k, m);	
		}

		//while((n = Read(sockfd, recvline, MAXLINE)) > 0) {
			recvline[n] = 0; /*null-term.*/
			Fputs(recvline, stdout);
		}
		exit(0);

	}
}
