#include "apue.h"

void pr_mask(const char *str) {
	sigset_t sigset;
	int errno_save;

	errno_save = errno;
	if(sigprocmask(0, NULL, &sigset) < 0)
		err_ret("sigprocmask error");
	else {
		printf("%s", str);

		if(sigismember(&sigset, SIGINT))
			printf(" SIGINT");
		if(sigismember(&sigset, SIGABRT))
			printf(" SIGABRT");
		if(sigismember(&sigset, SIGALRM))
			printf(" SIGALRM");
		if(sigismember(&sigset, SIGCHLD))
			printf(" SIGCHLD");
		if(sigismember(&sigset, SIGCONT))
			printf(" SIGCONT");
		if(sigismember(&sigset, SIGHUP))
			printf(" SIGHUP");
		if(sigismember(&sigset, SIGILL))
			printf(" SIGILL");
		if(sigismember(&sigset, SIGKILL))
			printf(" SIGKILL");
		if(sigismember(&sigset, SIGPIPE))
			printf(" SIGPIPE");
		if(sigismember(&sigset, SIGPOLL))
			printf(" SIGPOLL");
		if(sigismember(&sigset, SIGSEGV))
			printf(" SIGSEGV");
		if(sigismember(&sigset, SIGTERM))
			printf(" SIGTERM");
		if(sigismember(&sigset, SIGXFSZ))
			printf(" SIGXFSZ");
	}
}
