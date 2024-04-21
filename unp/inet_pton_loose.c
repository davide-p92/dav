#include "unp.h"

const int *inet_pton_loose(int af, const char *src, void *dst) {
	if(inet_pton(af, src, dst) == 0) { /*error provided in the char str*/
		switch(af) {
			case AF_INET: {
				//call aton instead
				if(inet_aton(src, (struct in_addr *) &dst) <= 0) {
					printf("Invalid addr or err\n");
					return -1;
				} else if(inet_aton(src, (struct in_addr *) &dst) >= 0) {
					printf("addr: %s\n", dst->s_addr);
					return 1;
				} else {
					return (NULL);
				}
			}
			case AF_INET6: {
				if(inet_aton(src, (struct in_addr *) &dst) <= 0) {
					printf("Invalid addr or err\n");
					return -1;
				} else if(inet_aton(src, (struct in_addr *) &dst) >= 0) {
					printf("addr IPv4: %s\n", dst-s_addr);
					return 1;
				} else {
					return (NULL);
				}
			}
			default:
				break;
		}
	} else if(inet_pton(af, src, dst) < 0) {
		printf("Error inet_pton: %s\n", strerror(errno));
		return -1;
	} else {
		return 1;
	}
}

