#include "unp.h"

void str_echo(int sockfd) {
	ssize_t n;
	char buf[MAXLINE];

again:
	while((n = read(sockfd, buf, MAXLINE)) > 0)
		Writen(sockfd, buf, n);
	if(n < 0 && errno == EINTR)
		goto again;
	else if(n < 0) {
		printf("str_echo error: %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}
