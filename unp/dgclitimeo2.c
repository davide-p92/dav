#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen) {
	int n;
	char sendline[MAXLINE], recvline[MAXLINE];
	struct timeval tv;

	tv.tv_sec = 5;
	tv.tv_usec = 0;
	Setsockopt(sockfd, SOL_SOCKET, SO_RCVTIMEO, &tv, sizeof(tv));

	while(Fgets(sendline, MAXLINE, strlen(sendline), 0, pservaddr, servlen) != NULL) {
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		n = recvfrom(sockfd, recvline, MAXLINE, 0, NULL, NULL);
		if(n < 0) {
			if(errno == EWOULDBLOCK) {
				fprintf(stderr, "socket timed out.");
				continue;
			} else {
				printf("recvfrom error: %s\n", strerror(errno));
				return -1;
			}
		}
		recvline[n] = 0; /*null-term*/	
		Fputs(recvline, stdout);
	}
}

