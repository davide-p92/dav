#include "apue.h"
#include <shadow.h>

struct spwd *getspwnam(const char *);

int main(int argc, char **argv) {
	if(argc != 2)
		err_quit("usage: %s <username>\n", argv[0]);
	char *uname;
	uname = (char *)malloc(256 * sizeof(char));
	strcpy(uname, argv[1]);
	printf("uname %s\n", uname);
	struct spwd *sptr = getspwnam(uname);
	printf("encr pwd: %s\n", sptr->sp_pwdp);
	printf("days before account exp: %d\n", sptr->sp_expire);

	free(uname);
	return 0;
}

struct spwd *getspwnam(const char *uname) {	
	struct spwd *sptr;

	setspent();
	while((sptr = getspent()) != NULL) {
	
		if(strcmp(uname, sptr->sp_namp) == 0)
			/* match found */
			break;
	}
	//printf("encr spasswd: %s\n", sptr->sp_pwdp);
	//printf("days before account exp: %d\n", sptr->sp_expire);
	endspent();

	return (sptr);

}
