#include "apue.h"
#include <dirent.h>
#include <limits.h>

/*function type called for each fname */
typedef int Myfunc(const char *path, const struct stat *stbuf, int type);

static Myfunc *myfunc;
static int myftw(char *path, Myfunc *);
static int dopath(Myfunc *pathname);

static long nreg, ndir, nblk, nchr, nfifo, nslink, nsock, ntot;

int main(int argc, char *argv[]) {
	int ret;

	if(argc != 2)
		err_quit("usage: %s <starting-path>\n", argv[0]);

	ret = myftw(argv[1], myfunc); /* does it all*/

	ntot = nreg + ndir + nblk + nchr + nfifo + nslink + nsock + ntot;
	if(ntot == 0)
		ntot = 1; /* avoid div by 0 */
	printf("regular files\t=%7ld, %5.2f %%\n", nreg, nreg*100.0/ntot);
	printf("directories\t=%7ld, %5.2f %%\n", ndir, ndir*100/ntot);
	printf("block special\t=%7ld, %5.2f %%\n", nblk, nblk*100/ntot);
	printf("char special\t=%7ld, %5.2f %%\n", nchr, nchr*100/ntot);
	printf("FIFOS\t=%7ld, %5.2f %%\n", nfifo, nfifo*100/ntot);
	printf("symbolic links\t=%7ld, %5.2f %%\n", nslink, nslink*100/ntot);
	printf("sockets\t=%7ld, %5.2f %%\n", nsock, nsock*100/ntot);
	exit(ret);
}

/*
 * Descend through the hierarchy start at "pathname"
 * func() is called for every file by caller.
 */
#define FTW_F	1	/*file other than dir*/
#define FTW_D	2	/*dir*/
#define FTW_DNR	3	/*dir that can't be read*/
#define FTW_NS	4	/*file that can't be stat*/

static char *fullpath;
static size_t pathlen;

static int myftw(char *pathname, Myfunc *func) {	
	/*ret whatever func rets*/
	fullpath = path_alloc(&pathlen);	/*malloc(PATH+1)*/
	if(pathlen <= strlen(pathname)) {
		pathlen = strlen(pathname) * 2;
		if((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed");
	}
	strcpy(fullpath, pathname);
	return(dopath(func));
}
/*
 * Descend through the hierarchy, starting at "fullpath"
 * If "fullpath" is other than dir, 
 * call func() and ret. 
 * For dir, call ourself recursively for each name in dir.
 */
static DIR *dp;

static int dopath(Myfunc *func) {
	/*ret whatever func rets*/
	struct stat statbuf;
	struct dirent *dirp;
	//DIR *dp;
	int ret, n;

	if(lstat(fullpath, &statbuf) < 0)
		return(func(fullpath, &statbuf, FTW_NS));
	if(S_ISDIR(statbuf.st_mode) == 0) /*not a dir*/
		return(func(fullpath, &statbuf, FTW_F));
	/*It's a dir if we reach this. Call func()
	 * then process each filename in dir
	 */
	if((ret = func(fullpath, &statbuf, FTW_D)) != 0)
		return ret;

	n = strlen(fullpath);
	if(n + NAME_MAX + 2 > pathlen) {	/*buff too short*/
		pathlen *= 2;
		if((fullpath = realloc(fullpath, pathlen)) == NULL)
			err_sys("realloc failed.");
	}
	fullpath[n++] = '/';
	fullpath[n] = 0; /*null term.*/
	
	if((dp = opendir(fullpath)) == NULL)
		return(func(fullpath, &statbuf, FTW_DNR));

	while((dirp = readdir(dp)) != NULL) {
		if(strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) 
			continue;
		strcpy(&fullpath[n], dirp->d_name);
		if((ret = dopath(func)) != 0)
			break;
	}
	fullpath[n-1] = 0;	/*erase everythingfrom slash on*/

	if(closedir(dp) < 0)
		err_ret("can't close dir %s", fullpath);
	return ret;
}

static int myfunc(const char *pathname, const struct stat *statptr, int type) {
	switch(type) {
		case FTW_F:
			switch(statptr->st_mode & S_IFMT) {
				case S_IFREG:
					nreg++;
					break;
				case S_IFBLK:
					nblk++;
					break;
				case S_IFCHR:
					nchr++;
					break;
				case S_IFIFO:
					nfifo++;
					break;
				case S_IFLNK:
					nslink++;
					break;
				case S_IFSOCK:
					nsock++;
					break;
				case S_IFDIR: 
				/*should have been taken care of by FTW_D*/
					err_dump("for S_IFDIR for %s", pathname);
			}
			break;
		case FTW_D:
			ndir++;
			break;
		case FTW_DNR:
			err_ret("can't read dir %s", pathname);
			break;
		case FTW_NS:
			err_ret("stat error for %s", pathname);
			break;
		default:
			err_dump("unknown type for %s", pathname);
	}
	return 0;
}
		

