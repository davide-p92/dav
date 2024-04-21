#include "unp.h"
#include <syslog.h>

#define MAXFD 64

extern int daemon_proc;

int daemon_init(const char *pname, int facility) {
	int i;
	pid_t pid;

	if((pid = Fork()) < 0)
		return -1;
	else if(pid)
		_exit(0);	/*parent terminates*/

	/*child 1 cont...*/
	if(setsid() < 0)	/*become session leader*/
		return -1;

	Signal(SIGHUP, SIG_IGN);
	if((pid = Fork()) < 0)
		return -1;
	else if(pid)
		_exit(0);	/*terminate child1*/

	/*child 2 contn...*/
	daemon_proc = 1;	/*for err_XXX functs*/

	chdir("/");

	/*close fds*/
	for(i = 0; i < MAXFD; i++)
		close(i);

	/*redirect stdin, stdout and stderr*/
	open("/dev/null", O_RDONLY);
	open("/dev/null", O_RDWR);
	open("/dev/null", O_RDWR);

	openlog(pname, LOG_PID, facility);

	return 0;	/*success*/
}
