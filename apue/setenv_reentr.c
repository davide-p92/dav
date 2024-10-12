#include <limits.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <errno.h>

#define MAXSTRINGSZ 4096

static pthread_key_t key;
static pthread_once_t init_done = PTHREAD_ONCE_INIT;
pthread_mutex_t env_mutex = PTHREAD_MUTEX_INITIALIZER;

extern char **environ;

static void thread_init(void) {
	pthread_key_create(&key);
}

int setenv(const char *name, const char *value, int overwrite) {
	int i, len, len_val;
	char *envbuf;

	pthread_once(&init_done, thread_init);
	pthread_mutex_lock(&env_mutex);
	envbuf = (char *)pthread_getspecific(key); /*obtain addr of thread specific data*/
	/*look for var in environ*/
	int fnd = 0;
	int nvars = 0; //n. of existing variables
	len = strlen(name);
	len_val = strlen(value);
	if(len_val >= MAXSTRSZ - len) {
		//error, must also include null char
		errno = ENOBUFS;
		return -1;
	}
	for(i = 0; environ[i] != NULL; i++) {
		nvars++;
		if((strncmp(name, environ[i], len) == 0) && (environ[i][len] == '=')) {
			/*found variable, overwrite?*/
			fnd = 1;
			if(overwrite == 1) {
				//set envbuf to length of value and overwrite
				//int len_val = strlen(value
				if(envbuf == NULL) {
					envbuf = (char *) malloc(MAXSTRSZ - len);
					if(envbuf == NULL) {
						pthread_mutex_unlock(&env_mutex);
						//error, return
						errno = ENOBUFS;
						return -1;
					}
				}

				memcpy(envbuf, &value, sizeof(value));

				//write null char
				strncat(envbuf, '\0', 1);
				//copy to environ value				
				strncpy(envbuf, &environ[i], environ, MAXSTRSZ - len);
				pthread_mutex_unlock(&env_mutex);

			} else if(overwrite == 0) {
				return 0; //succes
				break; //leavee it be
			} else {
				//arg error
				errno = EINVAL;
				return -1;
			}
		}
	}
	//not found name, add it to environ arr
	if(fnd == 0) {
		if(envbuf == NULL) {
			envbuf = (char *) malloc(MAXSTRSZ - len);
			if(envbuf == NULL) {
				pthread_mutex_unlock(&env_mutex);
				errno = ENOBUFS;
				return -1;
			}
		}
		
		//write to envbuf then to environ[nvar]
		memcpy(envbuf, &name, sizeof(name));
		strncat(envbuf, '=', 1);
		strncat(envbuf, &value, sizeof(value));
		strncpy(&environ[nvar], envbuf, sizeof(envbuf));
		strcat(&environ[nvar], '\0');
		
		//unlock thread mutex variable
		pthread_mutex_unlock(&env_mutex);
	}
	//return.
	return 0;
}
