#include "hd.h"

static void error_dav(int, const char *, va_list);

void print_err(int error, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	error_dav(error, fmt, ap);
	va_end(ap);
	exit(1);
}

void print_exit(const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	error_dav(0, fmt, ap);
	va_end(ap);
	exit(1);
}

static void error_dav(int error, const char *fmt, va_list ap) {
	char buf[MAXLINE];
	vsnprintf(buf, MAXLINE - 1, fmt, ap);
	snprintf(buf + strlen(buf), MAXLINE - strlen(buf) - 1, " :%s", strerror(error));
	strcat(buf, "\n");
	fflush(stdout);
	fputs(buf, stderr);
	fflush(NULL);
}


