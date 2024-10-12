void fatal(char *message) {
	char err_message[100];

	strcpy(err_message, "[!!] Fatal Error ");
	strncat(err_message, message, 83);
	perror(err_message);
	exit(-1);
}

void *ec_malloc(unsigned int size) {
	void *ptr;
	ptr = malloc(size);
	if(ptr == NULL)
		fatal("in ec_malloc() on memory allocation");
	return ptr;
}
