#include "apue.h"
#include <errno.h>
#include <limits.h>
#include <sys/resource.h>

#ifdef OPEN_MAX
static long openmax = OPEN_MAX;
#else
static long openmax = 0;
#endif

/*
 * if OPEN_MAX is undeterminate (sysconf(OPEN_MAX) < 0 && errno unchanged
 */
#define OPEN_MAX_GUESS 256

long open_max(void) {
	if(openmax == 0) {
		errno = 0;
		openmax = sysconf(OPEN_MAX);
		struct rlimit rl;
		if((openmax < 0) || (openmax == LONG_MAX)) {
			if(getrlimit(RLIMIT_NOFILE, &rl) < 0) {
				printf("can't get file limit");
				return -1;
			}
			if(rl.rlim_max == RLIM_INFINITY)
			//if(errno == 0)
				openmax = OPEN_MAX_GUESS;
			else
				openmax = rl.rlim_max;
			//else {
			//	printf("sysconf error for OPEN_MAX, %s\n", strerror(errno));
			//	return -1;
			//}
		}
	}
	return openmax;
}
