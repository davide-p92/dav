#define _XOPEN_SOURCE 700

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ftw.h>
#include <unistd.h>

//Function to search for str in file
int search_str(const char *filename, const char *str) {
	int buf[1024];
	int linen = 1;
	int found = 0;
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		perror("Error opening file\n");
		return -1;
	}

	while(fgets(buf, sizeof(buf), file) != NULL) {
		if(strstr(buf, str) != NULL) {
			printf("Found '%s' in file %s, line %d\n", str, filename, linen);
			found++;
		}
		linen++;
	}
	fclose(file);
	return found;
}

//nftw callback function
int search_callback(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
	if(typeflag == FTW_F) {
		//Regular files allowed
		char *str = getenv("SEARCH_STRING"); //from environment var
		if(str == NULL) {
			printf("Please provide a valid search string as argument.\n");
			return -1;
		}
		if(search_str(fpath, str)) {
			printf("Hello word\n");
		}
	}
	return 0;
}

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s <dir> <string>\n", argv[0]);
		return 0;
	}
	char str[strlen(argv[2]) + 1];
       	strcpy(str,argv[2]);
	char dir[strlen(argv[1]) + 1];
       	strcpy(dir, argv[1]);
	setenv("SEARCH_STRING", str, 1);
	int flags = FTW_PHYS; //Avoid symbolic links

	if(nftw(dir, search_callback, 20, flags) == -1) {
		perror("Nftw");
		exit(EXIT_FAILURE);
	}
	return 0;
}
