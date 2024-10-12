#include "unp.h"
#include "readline.c"

#ifdef HAVE_SOCKADDR_DL_STRUCT
#include <net/if_dl.h>
#endif


int Socket(int domain, int type, int protocol) {
	int sock = socket(domain, type, protocol);
	if (sock == -1) {
		printf("Error socket. %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
	return sock;
}

void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen) {
	int ret = bind(sockfd, addr, addrlen);
	if(ret == -1) {
		perror("Error bind.");
		exit(EXIT_FAILURE);
	}
}

void Listen(int sockfd, int listenq) {
	int ret = listen(sockfd, listenq);
	if(ret == -1) {
		printf("Error listen. %s\n", strerror(errno));
		exit(EXIT_FAILURE);
	}
}

int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen) {
	int ret = accept(sockfd, addr, addrlen);
	if(ret == -1) {
		printf("Error accept. %s\n", strerror(errno));
		return -1;
	} else {
		printf("Connection established\n");
		return ret;
	}
}

pid_t Fork() {
	if(fork() == -1) {
		printf("Error Fork. %s", strerror(errno));
		return -1;
	}
}


ssize_t readn(int fd, void *vptr, size_t n) {
	size_t nleft;
	ssize_t nread;
	char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if((nread = read(fd, ptr, nleft)) < 0) {
			if(errno == EINTR)
				nread = 0;
			else
				return -1;
		} else if(nread == 0) {
			break; /*EOF*/
		}
		nleft -= nread;
		ptr += nread;
	}
	return (n - nleft); //return >= 0
	
}

ssize_t Readn(int fd, void *vptr, size_t n) {
	if(readn(fd, vptr, n) == -1) {
		printf("error Readn: %s\n", strerror(errno));
		return -1;
	}
}

int writen(int fd, void *vptr, int n) {
	size_t nleft;
	ssize_t nwritten;
	const char *ptr;

	ptr = vptr;
	nleft = n;
	while(nleft > 0) {
		if((nwritten = write(fd, ptr, nleft)) <= 0) {
			if(nwritten < 0 && errno == EINTR)
				nwritten = 0;
			else
				return -1; /*error*/
		}
		nleft -= nwritten;
		ptr += nwritten; //increment ptr
	}
	return (long)(n - nleft);
}

int Writen(int sockfd, void *buf, int buflen) {
	if(writen(sockfd, buf, buflen) == -1) {
		printf("Error Writen. %s\n", strerror(errno));
		return -1;
	}
}

int Close(int sockfd) {
	if(close(sockfd) == -1) {
		printf("Error Close. %s\n", strerror(errno));
		return -1;
	}
}

int Inet_pton(int af, const char *src, void *dst) {
	if(inet_pton(af, src, dst) == -1) {
		printf("Error inet_pton. %s\n", strerror(errno));
		return -1;
	}
	else if(inet_pton(af, src, dst) == 0) {
		printf("Error inet_pton: invalid network address for family.\n");
		return 0;
	}
}

int Connect(int connfd, const struct sockaddr *addr, socklen_t addrlen) {
	if(connect(connfd, addr, addrlen) == -1) {
		printf("Error Connect %s\n", strerror(errno));
		return -1;
	}
	return 0;
}

char *Fgets(char *str, int n, FILE *stream) {
	char *res = fgets(str, n, stream);
	if(res == NULL) {
		if(feof(stream)) {
			fprintf(stderr, "Error: end of file reached.\n");
		} else if(ferror(stream)) {
			perror("Error occurred while reading\n");
		}
	}
	return res;
}

int Fputs(const char *str, FILE *stream) {
	int res = fputs(str, stream);
	if(res == EOF) {
		perror("Error writing to file.\n");
	}
	return res;
}

ssize_t Readline(int fd, void *vptr, size_t maxlen) {
	ssize_t res = readline(fd, vptr, maxlen);
	if(res == 0) {
		printf("Error. Readline yielded 0 (EOF)\n");
	} else if (res < 0) {
		printf("Error. Readline did not fill buffer.\n");
	}
	return res;
}

int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout) {
	int res = select(nfds, readfds, writefds, exceptfds, timeout);
	if(res == -1) {
		perror("Select error");
		if(errno == EBADF) {
			printf("Bad file select.\n");
	       		return -1;
	 	} else if(errno == EINVAL) {
			printf("Invalid argument select.\n");
			return -1;
		}	
	}
	return res;
}

int Shutdown(int sockfd, int how) {
	int res;
	if((res = shutdown(sockfd, how)) == -1) {
		perror("Shutdown error.\n");
		if(errno == EBADF) {
			printf("shutdown: bad file.\n");
		} else if (errno == ENOTCONN) {
			printf("shutdown: not connected error.\n");
		}
	}
	return res;
}

ssize_t Recvfrom(int sockfd, void *mesg, size_t buflen, int flags, struct sockaddr *src_addr, socklen_t *addrlen) {
	ssize_t n;
	n = recvfrom(sockfd, mesg, buflen, flags, src_addr, addrlen);
	if(n < 0) {
	printf("Error recvfrom: %s\n", strerror(errno));
		return -1;
	} else if (n == 0) {
		printf("recvfrom: EOF\n");
	}
	return n;
}

ssize_t Sendto(int sockfd, const void *buf, size_t buflen, int flags, const struct sockaddr *dst_addr, socklen_t addrlen) {
	ssize_t n;
	if((n = sendto(sockfd, buf, buflen, flags, dst_addr, addrlen) < 0)) {
		printf("Error sendto: %s\n", strerror(errno));
		return -1;
	}
	return n;
}

const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size) {
	const char *res;
	res = inet_ntop(af, src, dst, size);
	if(res == NULL) {
		printf("Error Inet_ntop: %s\n", strerror(errno));
		return NULL;
	}
	return res;
}


//Signal wrapper functoin
SignalHandlerFunction Signal(int signum, SignalHandlerFunction handler) {
	SignalHandlerFunction previousHandler;
	//Set signal handler and check
	if((previousHandler = signal(signum, handler)) == SIG_ERR) {
		perror("error signal");
		exit(EXIT_FAILURE);
	}

	return previousHandler;
}

char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen) {
	static char str[128];
	switch(sa->sa_family) {
	case AF_INET: {
		struct sockaddr_in *sin = (struct sockaddr_in *)sa;

		if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
			return (NULL);
		return str;
	}
#ifdef IPV6
	case AF_INET6: {
		struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
	
		if(inet_ntop(AF_INET6, &sin6->sin_addr, str, sizeof(str)) == NULL)
			return(NULL);
		return (str);
	}
#endif

#ifdef AF_UNIX
	case AF_UNIX: {
		struct sockaddr_un *unp = (struct sockaddr_un *)sa;
	       /*ok to have no pathname bound to sock: happens on every connect() unless client calls bind() first. */
		if(unp->sun_path[0] == 0)
			strcpy(str, "(no pathname bound)");
		else
			snprintf(str, sizeof(str), "%s", unp->sun_path);
		return(str);
	}
#endif

#ifdef HAVE_SOCKADDR_DL_STRUCT
	case AF_LINK: {
		struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;
  		if(sdl->sdl_nlen > 0)
			snprintf(str, sizeof(str), "%*s", sdl->sdl_nlen, &sdl->sdl_data[0]);
		else
			snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
		return (str);
	}
#endif

	default:
		snprintf(str, sizeof(str), "sock_ntop_host: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
		return (str);
	}
	return NULL;
}

char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen) {
	char *ptr;

	if((ptr = sock_ntop_host(sa, salen)) == NULL) {
		printf("sock_ntop_host error %s", strerror(errno));
		return NULL;
	}
	return ptr;
}

char *sock_ntop(const struct sockaddr *sa, socklen_t salen) {
	char portstr[8];
	static char str[128];	/*Unix domain largest*/

	switch(sa->sa_family) {
		case AF_INET: {
			struct sockaddr_in *sin = (struct sockaddr_in *)sa;
			if(inet_ntop(AF_INET, &sin->sin_addr, str, sizeof(str)) == NULL)
				return NULL;
			if(ntohs(sin->sin_port) != 0) {
				snprintf(portstr, sizeof(portstr), ":%d", ntohs(sin->sin_port));
				strcat(str, portstr);
			}
			return (str);
		}
#ifdef IPV6
		case AF_INET6: {
			struct sockaddr_in6 *sin6 = (struct sockaddr_in6 *)sa;
			str[0] = '[';
			if(inet_ntop(AF_INET6, &sin6->sin6_addr, str + 1, sizeof(str) - 1) == NULL)
				return NULL;
			if(ntohs(sin6->sin6_port) != 0) {
				snprintf(portstr, sizeof(portstr), "]:%d", ntohs(sin6->sin6_port));
				strcat(str, portstr);
				return str;
			}
			return(str + 1);
		}
#endif

#ifdef AF_UNIX
		case AF_UNIX: {
			struct sockaddr_un *unp = (struct sockaddr_un *)sa;
			if(unp->sun_path[0] == 0)
				strcpy(str, "(no pathname bound)");
			else
				snprintf(str, sizeof(str), "%s", unp->sun_path);
			return str;
		}
#endif

#ifdef HAVE_SOCKADDR_DL_STRUCT
		case AF_LINK: {
			struct sockaddr_dl *sdl = (struct sockaddr_dl *)sa;
			if(sdl->sdl_nlen > 0)
				snprintf(str, sizeof(str), "%*s (index %d)", sdl->sdl_nlen, &sdl->sdl_data[0], sdl->sdl_index);
			else
				snprintf(str, sizeof(str), "AF_LINK, index=%d", sdl->sdl_index);
			return str;
		}
#endif
		default: {
			snprintf(str, sizeof(str), "sock_ntop: unknown AF_xxx: %d, len %d", sa->sa_family, salen);
			return str;
		}
	}
	return NULL;
}

char *Sock_ntop(const struct sockaddr *sa, socklen_t salen) {
	char *ptr;

	if((ptr = sock_ntop(sa, salen)) == NULL)
		printf("sock_ntop error: %s\n", strerror(errno));
		
	return ptr;
}










