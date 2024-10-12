#include "apue.h"
#include <fcntl.h>

int set_cloexec(int fd) {
	int val;

	if((val = fcntl(fd, F_GETFD, 0)) < 0)
		return -1;

	val |= FD_CLOEXEC;
	/*
	 * so that if config files are open, on childe processes they are closed
	 * to avoid sec issues.
	 */

	return(fcntl(fd, F_SETFD, val));
}	
