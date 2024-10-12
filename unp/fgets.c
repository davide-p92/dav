#include "unp.h"

char *Fgets(char *str, int n, FILE *stream) {
	if(fgets(str, n, stream) == NULL) {
		printf("error Fgets: %s\n", strerror(errno));
		return -1;
	}
}
