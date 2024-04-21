#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdarg.h>
#include <errno.h>
#include <string.h>

#define MAXLINE 2048

void print_err(int, const char *, ...);

void print_exit(const char *, ...);

void read_file(const char *fname, char *lines[]);
