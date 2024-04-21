#define _XOPEN_SOURCE 500
#include <stdio.h>
#include <ftw.h>
#include <string.h>
#include <stdlib.h>
#include <sys/stat.h>
#define MAX_DEPTH 10


//int funct(const char *, const struct stat *, int, struct FTW *);
char *target_directory_name;

int func_callback(const char *fpath, const struct stat *statptr, int typeflag, struct FTW *ftwbuf) 
{
	char *fname = strrchr(fpath, '/');
	if(fname != NULL) {
		fname++;
		if(typeflag == FTW_D && strcmp(fname, target_directory_name) == 0)
		{
			printf("Found directory: %s.\n", fpath);
			return 0;
		}
	}
	return 0;
}

int main(int argc, char *argv[]) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <target_directory>\n", argv[0]);
		exit(EXIT_FAILURE);
	}

	target_directory_name = malloc(strlen(argv[1]) + 1);
	if(target_directory_name == NULL) {
		perror("malloc");
		exit(EXIT_FAILURE);
	}

	strcpy(target_directory_name, argv[1]);	
	if(nftw("/", func_callback, MAX_DEPTH, FTW_PHYS) == -1)//, FTW_DEPTH | FTW_PHYS) == -1) 
	{
		perror("nftw");
		exit(EXIT_FAILURE);
		printf("Directory '%s' not found in the filesystem.\n", target_directory_name);

	}

	free(target_directory_name);
	return 0;
}
