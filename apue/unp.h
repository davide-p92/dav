#ifndef UNP_H
#define UNP_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <arpa/inet.h> 
#include <signal.h>
#include <sys/types.h>
#include <sys/select.h>
#include <math.h>
#include <sys/param.h>
#include <netdb.h>
#include <sys/un.h>

#ifdef HAVE_SOCKADDR_DL_STRUCT
#include <net/if_dl.h>
#endif

#define MAXLINE 1024
#define LISTENQ 10
#define SERV_PORT 9877

int Socket(int domain, int type, int protocol);
void Listen(int sockfd, int listenq);
void Bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen);
int Accept(int sockfd, struct sockaddr *addr, socklen_t *addrlen);
int Connect(int connfd, const struct sockaddr *addr, socklen_t addrlen);
int Writen(int connfd, char *buf, int buflen);
ssize_t Readline(int fd, void *vptr, size_t maxlen);
pid_t Fork(void);
ssize_t readn(int fd, void *vptr, size_t n);
ssize_t Read(int fd, void *vptr, size_t n);
ssize_t writen(int fd, const void *vptr, size_t n);
int Close(int sockfd);
int Inet_pton(int af, const char *src, void *dst);
void str_echo(int sockfd);
void str_cli(FILE *fp, int sockfd);
void dg_echo(int sockfd, struct sockaddr *pcliaddr, socklen_t clilen);
char *Fgets(char *str, int n, FILE *stream);
int Fputs(const char *str, FILE *stream);
int Select(int nfds, fd_set *readfds, fd_set *writefds, fd_set *exceptfds, struct timeval *timeout);
typedef void (*SignalHandlerFunction)(int);
SignalHandlerFunction Signal(int signum, SignalHandlerFunction handler);
int Shutdown(int sockfd, int how);
ssize_t Recvfrom(int sockfd, void *buf, size_t buflen, int flags, struct sockaddr *src_addr, socklen_t *addrlen);
ssize_t Sendto(int sockfd, const void *buf, size_t buflen, int flags, const struct sockaddr *dst_addr, socklen_t addrlen);
const char *Inet_ntop(int af, const void *src, char *dst, socklen_t size);
char *sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
char *Sock_ntop_host(const struct sockaddr *sa, socklen_t salen);
int tcp_connect(const char *host, const char *serv);
int Tcp_connect(const char *host, const char *serv);
int Getpeername(int sockfd, struct sockaddr *addr, socklen_t *len);
#endif
