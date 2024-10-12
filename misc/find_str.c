#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

void search_in_file(const char *filename, const char *search_string);
void search_in_directory(const char *dir_name, const char *search_string);

void search_in_file(const char *filename, const char *search_string) {
	FILE *file = fopen(filename, "r");
	if(file == NULL) {
		//skip file
		return;
	}

	char line[1024];
	int line_number = 1;

	while(fgets(line, sizeof(line), file) != NULL) {
		if(strstr(line, search_string) != NULL) {
			printf("Found in file: %s, line %d: %s", filename, line_number,  line);
		}
		line_number++;
	}

	fclose(file);
}

void search_in_directory(const char *dir_name, const char *search_string) {
	DIR *dir = opendir(dir_name);
	if(dir == NULL) {
		//skip dir
		return;
	}
	
	struct dirent *entry;
	while((entry = readdir(dir)) != NULL) {
		//skip "." and ".." entries
		if(strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
			continue;
		}

		char path[4096];
		snprintf(path, sizeof(path), "%s/%s", dir_name, entry->d_name);

		struct stat path_stat;
		if(stat(path, &path_stat) == 0) {
			if(S_ISDIR(path_stat.st_mode)) {
				//recursively search insidedir
				search_in_directory(path, search_string);
			} else if(S_ISREG(path_stat.st_mode)) {
				//if file, search file
				search_in_file(path, search_string);
			}
		}
	}
	
	closedir(dir);
}

int main(int argc, char **argv) {
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <search_string>\n", argv[0]);
		return EXIT_FAILURE;
	}

	const char *search_string = argv[1];

	search_in_directory("/", search_string);

	return EXIT_SUCCESS;
}
