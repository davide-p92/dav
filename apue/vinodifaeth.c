/* vinodifaeth.c */
#define _DEFAULT_SOURCE //per definire struct ifr in net/if.h
#include "apue.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <linux/if_packet.h>
#include <linux/if_ether.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <sys/ioctl.h>
#include <netdb.h> /*getprotoent to read protoent.p_name, protoent.p_aliases, 
		     protoent.p_proto from /etc/protocols*/
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>
#include <syslog.h>
#include <signal.h>
#include <string.h>

#define MAXBUF 4096

#ifndef IFF_PROMISC
#define IFF_PROMISC 0x100
#endif
#ifndef IFF_SLAVE
#define IFF_SLAVE 0x800
#endif
#ifndef IFF_PORTSEL
#define IFF_PORTSEL 0x2000
#endif
#ifndef IFNAMSIZ
#define IFNAMSIZ IF_NAMESIZE
#endif

int sock_setup(int sockfd, int socktype) {
	//set up raw socket (i.e. with eth headrs) as well as specify the type
	//and handle errs
	if((sockfd = socket(PF_INET, socktype, htons(ETH_P_ALL))) < 0) {
		printf("error socket: %s\n", strerror(errno));
		syslog(LOG_ERR, "error socket: %s", strerror(errno));
		return -1;
	}
	return sockfd;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("usage: %s <interface name>\n", argv[0]);
		return -1;
	}
	//data initialization
	struct sockaddr_ll sinfo;
	struct ifreq ifrfl, ifraddr, ifrhw, ifrmap;
	struct protoent *protoeth, *protol2tp, *protoeip;
	int sockfd;
	protoeth = getprotobyname("ethernet");
	protol2tp = getprotobyname("l2tp");
	protoeip = getprotobyname("etherip");
	sockfd = sock_setup(sockfd, SOCK_PACKET);
	ifrfl.ifr_addr.sa_family = AF_INET;
	ifraddr.ifr_addr.sa_family = AF_INET; //we want an IP addr
	ifrhw.ifr_addr.sa_family = AF_UNSPEC;
	ifrmap.ifr_addr.sa_family = AF_INET;
	
	/*I want IP address attached to 'argv[1]'
	 *
	 */
	strncpy(ifraddr.ifr_name, argv[1], IFNAMSIZ-1);
	strncpy(ifrhw.ifr_name, argv[1], IFNAMSIZ-1);
	strncpy(ifrmap.ifr_name, argv[1], IFNAMSIZ-1);
	strncpy(ifrfl.ifr_name, argv[1], IFNAMSIZ-1);

	int flagcmden[4] = {SIOCGIFFLAGS, SIOCGIFADDR, SIOCGIFHWADDR /*| SIOCGIFHWBROADCAST*/, SIOCGIFMAP};
	//call and check for each cmd

	if(ioctl(sockfd, SIOCGIFFLAGS, &ifrfl) < 0) {
		printf("error ioctl: %s\n", strerror(errno));
		//		return -1;
	}
				//break;
			//case flagcmden[1]:
	if(ioctl(sockfd, SIOCGIFADDR, &ifraddr) < 0) {
		printf("error ioctl: %s\n", strerror(errno));
	//		return -1;
	}
		//break;
	
	if(ioctl(sockfd, SIOCGIFHWADDR, &ifrhw) < 0) {
		printf("error ioctl: %s\n", strerror(errno));
	//return -1;
	}
	
	if(ioctl(sockfd, SIOCGIFMAP, &ifrmap) < 0) {
		printf("error ioctl: %s\n", strerror(errno));
		//return -1;
	}

	close(sockfd);
	printf("%x, %x, %x, %x", &ifraddr, &ifrfl, &ifrhw, &ifrmap);
	printf("ip address: %s\n", inet_ntoa(((struct sockaddr_in *)&ifraddr.ifr_addr)->sin_addr));
	printf("ip dst address: %s\n",inet_ntoa(((struct sockaddr_in *)&ifraddr.ifr_dstaddr)->sin_addr));
	printf("ip broad addr: %s\n", inet_ntoa(((struct sockaddr_in *)&ifraddr.ifr_broadaddr)->sin_addr));
	printf("hw address: %02x:%02x:%02x:%02x:%02x:%02x\n",
		       (unsigned char) ifrhw.ifr_hwaddr.sa_data[0],
	       		(unsigned char) ifrhw.ifr_hwaddr.sa_data[1],
	 		(unsigned char) ifrhw.ifr_hwaddr.sa_data[2],
			(unsigned char) ifrhw.ifr_hwaddr.sa_data[3],
			(unsigned char) ifrhw.ifr_hwaddr.sa_data[4],
			(unsigned char) ifrhw.ifr_hwaddr.sa_data[5]);		
	//print flags
	int iffprom, iffslav, iffportsel;
	iffprom = ifrfl.ifr_flags & IFF_PROMISC;
	iffslav = ifrfl.ifr_flags & IFF_SLAVE;
	iffportsel= ifrfl.ifr_flags & IFF_PORTSEL;
	printf("if flags: [IFF_PROMISC] %d, [IFF_SLAVE] %d, [IFF_PORTSEL] %d\n", iffprom, iffslav, iffportsel);
	//print map
	printf("if maps: [mem_start] %ld, [mem_end] %ld, [base_addr] %x, [irq] %c, [dma] %c, [port] %c\n", ifrmap.ifr_map.mem_start, ifrmap.ifr_map.mem_end,ifrmap.ifr_map.base_addr,ifrmap.ifr_map.irq, ifrmap.ifr_map.dma,ifrmap.ifr_map.port); 
	
	//return
	return 0;
}
