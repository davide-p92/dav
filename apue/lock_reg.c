#include "apue.h"
#include <fcntl.h>

int lock_reg(int fd, int cmd, int type, off_t oset, int whence, off_t len) {
	struct flock lck;
	lck.l_type = type; /*F_RDLCK, F_WRLCK, F_UNLCK*/
	lck.l_start = oset;
	lck.l_whence = whence;
	lck.l_len = len;
	
	return fcntl(fd, cmd, &lck);
}
 
