#include "unp.h"
#include <sys/wait.h>

void (*signal(int signo, void (*func)(int)))(int) {
	struct sigaction act, oact;
	act.sa_handler = func;
	sigemptyset(&act.sa_mask);
	act.sa_flags = 0;
	if(signo == SIGALRM) {
#ifdef SA_INTERRUPT
		act.sa_flags |= SA_INTERRUPT;
#endif
	} else {
#ifdef SA_RESTART
		act.sa_flags |= SA_RESTART;
#endif
	}
	if(sigaction(signo, &act, &oact) < 0)
		return SIG_ERR;
	return (oact.sa_handler);
}

void sig_chld(int signo) {
	pid_t pid;
	int stat;

	//pid = wait(&stat);
	//to avoid not killing children whose signals are not queued
	//and avoiding leaving zombies of connected fds
	while((pid = waitpid(-1, &stat, WNOHANG)))
		printf("Child %d terminated\n", pid);
	return;
}
