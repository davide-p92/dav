#include "apue.h"

#define MAXL 4

int is_unbuffered(FILE *);

int main(void) {
	char buf[MAXL];
//	int is_unbuffered;

	while(fgets(buf, MAXL, stdin) != NULL) {
		if(fputs(buf, stdout) == EOF)
			err_sys("output error");
		if(is_unbuffered(stdin))
			printf("stdin is unbuffered\n");
		else
			printf("stdin is buffered\n");
		if(is_unbuffered(stdout))
			printf("stdout is unbuffered\n");
		else
			printf("stdout is buffered\n");	
		if(ferror(stdin))
			err_sys("input error");

	}
	exit(0);
}
#ifndef _IO_UNBUFFERED
#define _IO_UNBUFFERED _IONBF
#endif
int is_unbuffered(FILE *fp) {
	return (fp->_flags & _IO_UNBUFFERED);
}
