#include "unp.h"

static void connect_alarm(int);

int connect_timeo(int sockfd, const struct sockaddr *saptr, socklen_t salen, int nsec) {
	Sigfunc *sigfunc;
	int n;

	sigfunc = Signal(SIGALRM, connect_alarm);

	if ((n = connect(sockfd, saptr, salen)) < 0) {
		close(sockfd);
		if(alarm(nsec) != 0) {
			printf("connect_timeo: alarm already set\n");
		}
		if(errno == EINTR) {
			errno = ETIMEDOUT;
		}
	}
	alarm(0); //Turn off the alarm
	Signal(SIGALRM, sigfunc);

	return n;
}

static void connect_alarm(int signo) {
	return;
}
