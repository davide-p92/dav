#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <poll.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <net/if.h>
#include <ifaddrs.h>
#include <net/ethernet.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h> //sll_hatype
#include <linux/if_ether.h> //protocols


int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: %s <ifname>\n", argv[0]);
		return 0;
	}

	struct ifreq ifr;

	struct sockaddr_ll ifaddrll;
	memset(&ifaddrll, 0, sizeof(ifaddrll));
	ifaddrll.sll_family = AF_PACKET;
	//structure to call on recvmsg, read with cmsg
	struct tpacket_auxdata taux;
	//st
	struct tpacket_stats tstat;
	//hold both IPv4 and IPv6 addr
	struct sockaddr_storage saddr_st;
	int sockfd;
	//interface name
	char ifname[IFNAMSIZ];
	strcpy(ifname, argv[1]);
	printf("ifname: %s\n", ifname);
	//make socket accept connections from eth dev
	//char protoname[10];
	struct protoent *prent;
	prent = getprotobyname("ethernet");
	int protocol;
	protocol = prent->p_proto;
	printf("prent: %d\n", prent->p_proto);
	if((sockfd = socket(AF_PACKET, SOCK_RAW, htons(protocol))) < 0) {
		printf("Error socket packet. %s\n", strerror(errno));
		return -1;
	}

	//search for input if name
	struct ifaddrs *ifaddr, *ifa;
	
	getifaddrs(&ifaddr);

	//if index
	unsigned int ii;

	for(ifa = ifaddr; ifa != NULL; ifa = ifa->ifa_next) {
		//if(ifa->ifa_addr) {
		printf("ifa->ifa_name %s\n", ifa->ifa_name);
		if(strcmp(ifa->ifa_name, ifname) == 0) {	
			printf("Found if : %s\n", ifa->ifa_name);				//strncpy(ifr.ifr_newname, ifa->ifa_name, IFNAMSIZ - 1);
			//look for IP of attached devices

			ii = if_nametoindex(ifa->ifa_name);
				/*if(ioctl(sockfd, SIOCSIFINDEX, &ifr) < 0) {
					printf("Error ioctl SIOCGIFINDEX: %s\n", strerror(errno));
					return -1;
				}*/
				//ifr.ifr_ifindex = ii;
			printf("ifr->ifr_ifindex: %d\n!", ii);
			
		//}

			if(ioctl(sockfd, SIOCGIFNAME, &ifr) < 0) {
				printf("Error ioctl SIOCGIFNAME: %s %s\n", strerror(errno), ifr.ifr_name);
				return -1;
			}
			if(ioctl(sockfd, SIOCGIFINDEX, &ifr) < 0) {
				printf("Error ioctl SIOCGIFINDEX: %s\n", strerror(errno));
				return -1;
			}
			//ioctl(sockfd, SIOCGHWADDR, &ifr);
			if(ioctl(sockfd, SIOCGIFADDR, &ifr) == -1) {
				printf("Error ioctl SIOCGIFADDR: %s\n", strerror(errno));
				return -1;
			}
			if (ioctl(sockfd, SIOCGIFFLAGS, &ifr) < 0) {
				printf("Error ioctl SIOCGIFFLAGS: %s\n", strerror(errno));
				return -1;
			}
			//set iface to promisc if not already
			if(ifr.ifr_flags & IFF_PROMISC == 0) {
				ioctl(sockfd, SIOCSIFFLAGS, &ifr);
				ifr.ifr_flags |= IFF_PROMISC;
			}
			//iniz info
			ifaddrll.sll_ifindex = ifr.ifr_ifindex;
			ifaddrll.sll_protocol = protocol;	
			//set socket options to retrieve also statistics
			const int enable = 1;
			if(setsockopt(sockfd, SOL_SOCKET, PACKET_STATISTICS, &enable, sizeof(int)) < 0) {
				printf("Error setsockopt: %s\n", strerror(errno));
				return -1;
			}
	
			//Bind sock to if
			if(bind(sockfd, (struct sockaddr *)&ifaddr, sizeof(ifaddr)) < 0) {
				printf("Error bind: %s\n", strerror(errno));
				return -1;
			}
			//out data
			printf("ifr.ifr_name: %s, ifaddrll.sll_protocol: %d, ifaddrll.sll_pktype: %c, ifaddrll.sll_addr: %s\n", ifr.ifr_name, ifaddrll.sll_protocol, ifaddrll.sll_pkttype, ifaddrll.sll_addr);
	//receive
	//det bufsz
			int n;
			unsigned int m = sizeof(n);
			getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, (void *)&n, &m);
			char buf[n];
			socklen_t ifaddrlen;
			ifaddrlen = sizeof(ifr.ifr_addr);
			recvfrom(sockfd, buf, n, MSG_WAITALL, (struct sockaddr *)&ifr.ifr_addr, &ifaddrlen); 
		}
	}
	//closesock
	close(sockfd);

	//free ifaddrs
	freeifaddrs(ifa);
	freeifaddrs(ifaddr);
	return 0;
}

