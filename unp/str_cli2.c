#include "unp.h"

void str_cli(FILE *fp, int sockfd) {
	int maxfdp1, stdineof;
	fd_set rset;
	char buf[MAXLINE];
	int n;

	stdineof = 0;
	FD_ZERO(&rset);
	for( ; ; ) {
		if(stdineof == 0)
			FD_SET(fileno(fp), &rset);	
		FD_SET(sockfd, &rset);
		maxfdp1 = MAX(fileno(fp), sockfd) + 1;
		Select(maxfdp1, &rset, NULL, NULL, NULL);

		if(FD_ISSET(sockfd, &rset)) {/*socket is readable*/
			if((n = readn(sockfd, buf, MAXLINE)) == 0) {
				if(stdineof == 1)
					return; /*normal termination*/
				else {
					printf("Error Read %s.\n", strerror(errno));
					exit(EXIT_FAILURE);
				}
			}
			Writen(fileno(stdout), buf, n);
		}
		if(FD_ISSET(fileno(fp), &rset)) { /*input is readable*/
			if((n = readn(fileno(fp), buf, MAXLINE)) == 0) {
				stdineof = 1;
				Shutdown(sockfd, SHUT_WR); /*send FIN*/
				FD_CLR(fileno(fp), &rset);
				continue;
			}
			Writen(sockfd, buf, n);
		}
	}
}
