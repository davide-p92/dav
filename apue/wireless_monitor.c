#include "apue.h"
#include <net/if.h>
#include <linux/wireless.h>

int main() {
	int sockfd;
	struct iwreq wreq;
	char ssid[32];

	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	if(sockfd < 0) {
		perror("socket");
		return 1;
	}

	memset(&wreq, 0, sizeof(struct iwreq));
	strncpy(wreq.ifr_name, "wlan0", IFNAMSIZ);

	//Getting SSID
	wreq.u.essid.pointer = ssid;
	wreq.u.essid.length = 32;
	if(ioctl(sockfd, SIOCGIWESSID, &wreq) < 0) {
		perror("ioctl");
		return 1;
	}

	printf("Connected SSID: %s\n", ssid);
	close(sockfd);
	return 0;
}



