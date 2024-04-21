#include "unp.h"
#include <syslog.h>

extern int daemon_proc;		/*defined in error.c*/

void daemon_inetd(const char *pname, int facility) {
	daemon_proc = 1;	/*for err_XXX() funcs*/
	openlog(pname, LOG_PID, facility);
}
