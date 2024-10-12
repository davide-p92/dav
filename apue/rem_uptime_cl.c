/*
 * program that allows client to communicate with server to obtain output
 * of system ìuptimeì cmd.
 */
#include "apue.h"
#include <netdb.h>
#include <errno.h>
#include <sys/socket.h>

#define BUFLEN 128

extern int connect_retry(int, int, int, const struct sockaddr *, socklen_t);

void print_uptime(int sockfd) {
	int n;
	char buf[BUFLEN];

	while((n = recv(sockfd, buf, BUFLEN, 0)) > 0)
		write(STDOUT_FILENO, buf, n);
	if(n < 0)
		/*error*/
		err_sys("recv error");

}

int main(int argc, char **argv) {
	struct addrinfo *ailist, *aip;
	struct addrinfo hint;
	int sockfd, err;

	if(argc != 2)
		err_quit("Usage: ruptime <hostname>");
	//allocate hint
	memset(&hint, 0, sizeof(hint));
	hint.ai_socktype = SOCK_STREAM;
	hint.ai_canonname = NULL;
	hint.ai_addr = NULL;
	hint.ai_next = NULL;

	if((err = getaddrinfo(argv[1], "ruptime", &hint, &ailist)) != 0)
		err_quit("getaddrinfo error: %s\n", gai_strerror(errno));

	/*
	 * ailist : addrinfo(s) returned, may be more than one in case the network is multihomed
	 * (i.e. accepts AF_INET & AF_INET6 protocols) or if services can
	 * be reached from multiple socket types (e.g. SOCK_STREAM & SOCK_DGRAM
	 */
	for(aip = ailist; aip != NULL; aip = aip->ai_next) {
		if((sockfd = connect_retry(aip->ai_family, SOCK_STREAM, 0, aip->ai_addr, aip->ai_addrlen)) < 0) {
			err = errno;
		} else {
			print_uptime(sockfd);
			exit(0);
		}
	}
	err_exit(err, "cannot conn to %s", argv[1]);
}	
