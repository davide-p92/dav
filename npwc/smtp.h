/* smtp.h */
#include <arpa/inet.h>
#include <errno.h>
#include <netdb.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include <ctype.h>
#include <stdarg.h>

#define MAXINPUT 512
#define MAXRESPONSE 1024
