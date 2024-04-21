#include "unp.h"
#include <stdarg.h>

static void err_doit(int, const char *, va_list);
char *pname = NULL;	/*caller can set this from argv[0]*/

void err_msg(const char *fmt, ...) {
	va_list ap;

	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	return;
}

void err_quit(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	err_doit(0, fmt, ap);
	va_end(ap);
	exit(1);
}

static void err_doit(int errnoflag, const char *fmt, va_list ap) {
	int errno_save;
	char buf[MAXLINE];

	errno_save = errno;	/*value caller might want printed*/
	vsprintf(buf, fmt, ap);
	if(errnoflag)
		sprintf(buf+strlen(buf), ": %s", strerror(errno_save));
	strcat(buf, "\n");
	fflush(stdout);	/*if stdout and stderr are the same*/
	fputs(buf, stderr);
	fflush(stderr);
	return;
}
