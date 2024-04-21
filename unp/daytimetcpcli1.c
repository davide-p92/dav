#include "unp.h"

int main(int argc, char **argv) {
	int sockfd, n;
	char recvline[MAXLINE + 1];
	struct sockaddr_in servaddr;
	struct sockaddr_in6 servaddr6; /*IPv6 version*/
	struct in_addr **pptr;
	strutc in6_addr **pptr6; /*IPv6 vers*/
	struct in_addr *inetaddrp[2];
	struct in_addr inetaddr;
	struct hostent *hp;
	struct servent *sp;

	if(argc != 3) {
		printf("usage: %s <hostname> <service>\n", argv[0]);
		return -1;
	}

	if((hp = gethostbyname(argv[1])) == NULL) {
		if(inet_aton(argv[1], &inetaddr) == 0) {
			printf("hostname error for %s: %s\n", argv[1], hstrerror(h_errno));
			return -1;
		} else {
			inetaddrp[0] = &inetaddr;
			inetaddrp[1] = NULL;
			pptr = inetaddrp;
		}
	} else {
		pptr = (struct in_addr **) hp->h_addr_list;
	}

	if((sp = getservbyname(argv[2], "tcp")) == NULL) {
		printf("error getservbyname for %s", argv[2]);
		return -1;
	}
	
	/*IPv4*/
	if(strstr(argv[1], ":") == NULL) {
		for( ; *pptr != NULL; pptr++) {
			sockfd = Socket(AF_INET, SOCK_STREAM, 0);

			bzero(&servaddr, sizeof(servaddr));
			servaddr.sin_family = AF_INET;
			servaddr.sin_port = sp->s_port;
			memcpy(&servaddr.sin_addr, *pptr, sizeof(struct in_addr));
			printf("trying %s\n", Sock_ntop((struct sockaddr *) &servaddr, sizeof(servaddr)));
			if(connect(sockfd, (struct sockaddr *)&servaddr, sizeof(servaddr)) == 0)
				break; /*success*/
			printf("connect error");
			Close(sockfd);
			return -1;
		}
		if(pptr == NULL) {
			printf("unable to connect");
			return -1;
		}
	} else {
		for( ; *pptr6 != NULL; pptr6++) {
			sockfd = Socket(AF_INET6, SOCK_STREAM, 0);

			bzero(&servaddr6, sizeof(servaddr6));
			servaddr6.sin6_family = AF_INET6;
			servaddr6.sin6_port = s_port;
			memcpy(&servaddr6.sin6_addr, *pptr, sizeof(struct in6_addr));
			printf("trying %s\n", Sock_ntop((struct sockaddr *)&servaddr6, sizeof(servaddr6)));
			if(connect(sockfd, (struct sockaddr *)&servaddr,. sizeof(servaddr)) == 0)
				break; /*success vers 6*/
			printf("connect6 error");
			Close(sockfd);
			return -1;
		}
		if(pptr6 == NULL) {
			printf("unable to connect");
			return -1;
		}
	}
	while((n = Read(sockfd, recvline, MAXLINE)) > 0) {
		recvline[n] = 0; /*nullterminate*/
		Fputs(recvline, stdout);
	}

	return 0;
}
