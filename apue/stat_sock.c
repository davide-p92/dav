#include "apue.h"
#include <sys/stat.h>
#include <sys/socket.h>
#include <sys/type.h>
#include <fcntl.h>
#include <netdb.h>
#include <strings.h>

#define MAX 1024


int main(int argc, char **argv) {
	char buff[MAX];
	int sockfd;
	struct stat sb;
	struct sockaddr_in sa;
	if((sockfd = socket(AF_INET6, SOCK_STREAM, 0)) < 0) {
		err_sys("error socket");
	}
	bzero(&sa, sizeof(sa));
	sa.sin_family = AF_INET6;
	sa.sin_addr.s_addr = htonl(INADDR_ANY);
	sa.sin_port = htons(1234);

	if(bind(sockfd, (struct sockaddr *)&sa, sizeof(sa)) < 0) {
		err_sys("error bind");
	} else {
		printf("socket successfully binded to %s\n", inet_ntop(sa.sa_addr.s_addr));
	}
	if(
