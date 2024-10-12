#include <stdio.h>

int main() {
	char *dir;
	if ((dir = getenv("PX_IPC_NAME")) == NULL)
		return NULL;
#ifdef POSIX_IPC_PREFIX
	dir = POSIX_IPC_PREFIX;
#else
	dir = "/tmp/";
#endif
	printf("dir = %s\n", dir);
	return 0;
}
