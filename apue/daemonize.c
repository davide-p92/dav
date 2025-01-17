#include "apue.h"
#include <syslog.h>
#include <fcntl.h>
#include <sys/resource.h>

void daemonize(const char *cmd) {
	int i, fd0, fd1, fd2;
	pid_t pid;
	struct rlimit rlim;
	struct sigaction sa;

	/*
	 * clear file creation mask
	 */
	umask(0);
	/*
	 * get max nr of fds
	 */
	if(getrlimit(RLIMIT_NOFILE, &rlim) < 0)
		err_quit("%s: can't get file limit", cmd);

	/*
	 * become a session leader to lose controlling TTY.
	 */
	if((pid = fork()))
		err_quit("%s: can't fork", cmd);
	else if(pid != 0)
		exit(0);
	setsid(); /*start new session as process leadr*/

	/*
	 * ensure future opens won't allocate ctrl TTYs.
	 */
	sa.sa_handler = SIG_IGN;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = 0;
	if(sigaction(SIGHUP, &sa, NULL) < 0)
		err_quit("%s:can't ignore SIGHUP");
	if((pid = fork()) < 0)
		err_quit("%s: can't fork", cmd);
	else if(pid != 0) /*parent*/
		exit(0);

	/*
	 * change cwd to root '/', so we won't
	 * prevent file sys from being unmntd
	 */
	if(chdir("/") < 0)
		err_quit("%s: can't change directory to '/'", cmd);

	/*
	 * close all open fds
	 */
	if(rlim.rlim_max == RLIM_INFINITY)
		rlim.rlim_max = 1024;
	for(i = 0; i < rlim.rlim_max; i++)
		close(i);
	/*
	 * attach fds 0, 1 and 2 to /dev/null
	 */
	fd0 = open("/dev/null", O_RDWR);
	fd1 = dup(0);
	fd2 = dup(0);

	/*
	 * initialize log file
	 */
	openlog(cmd, LOG_CONS, LOG_DAEMON);
	if(fd0 != 0 || fd1 != 1 || fd2 != 2) {
		syslog(LOG_ERR, "unexpected file descriptors %d, %d, %d", fd0, fd1, fd2);
		exit(1);
	}
}
