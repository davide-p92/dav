#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>



int len_f(char *);

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: %s <file>\n", argv[0]);
		return -1;
	}
	char fname[256];
	int len;
	strcpy(fname, argv[1]);
	if((len = len_f(fname)) < 0) {
		printf("error len_f\n");
		return -1;
	}
	printf("Length of file %s: %d\n", fname, len);
	//flush to avoid strange behavior of terminal after call
	fflush(stdout);
	return 0;
}
//Function to read a file and store length
int len_f(char *fname) {
	char buf[1024];
	char **lines;
	char c;
	int nlin, nc, lenl, i;
	size_t nread;
	FILE *f;
	if((f = fopen(fname, "r")) == NULL) {
		printf("error fopen: %s\n", strerror(errno));
		return -1;
	}

	int lenf = 0;
	nlin = 0;
	i = 0;
	char line[1024];
	lines = (char **)malloc(sizeof(char *) * 4096);
	//memset(line, 0, sizeof(line));
	while((fgets(line, sizeof(line), f)) != NULL) {
		printf("line: %s\n", line);
		size_t len = strlen(line);
		lenf += len;
		/*allocate space for all lines*/
		if(len > 0 && line[len - 1] == '\n') {
			printf(line);
			lines[nlin] = line;
			printf("line: %d\tlength of line: %d\n", nlin, len);
			
			printf("lines[nlin] = %s\n", lines[nlin]);
			nlin++;
		}
	}
	fclose(f);
	//free memory
	free(lines);
	f = NULL;
	return lenf;
}

