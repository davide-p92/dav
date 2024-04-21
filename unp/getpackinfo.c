#include "unp.h"
#include <sys/ioctl.h>
#include <net/if.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>

#define BUF_SIZE 65536


int main(int argc, char **argv) {
	int s; //socket fd
	if(argc != 2) {
		printf("Usage: %s <dev-name>\n", argv[0]);
		return -1;
	}
	char *dev = argv[1];
	struct ifreq ifr; //Interface request strucrt
	char buf[BUF_SIZE]; //Buf to store packet
	struct sockaddr_in src; /*The source addr of sender*/
	socklen_t src_len = sizeof(src);
	struct sockaddr_ll addr;
	//skbuff to  hold packet
	struct sk_buff *skb;
	struct net_device *ndev;
	//pointer to eth headr
	struct ethhdr *eh;
	char *data;
	int len; /*The recvd packet len */
	int data_len;

	//Create a socket
	s = Socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	//Bind socket to network dev
	strncpy(ifr.ifr_name, dev, IFNAMSIZ);
	
	//Adapt the addr struct
	memset(&addr, 0, sizeof(addr));
	addr.sll_family = AF_PACKET;
	addr.sll_protocol = htons(ETH_P_ALL); /*The if prot*/
	addr.sll_ifindex = ifr.ifr_ifindex; /*The if index*/

	Bind(s, (struct sockaddr *)&addr, sizeof(addr));

	//Set network dev to promiscuous mode
	if(ioctl(s, SIOCGIFFLAGS, &ifr) < 0) {
		printf("ioctl SIOCGIFFLAGS\n");
		return -1;
	}
	ifr.ifr_flags |= IFF_PROMISC;
	if(ioctl(s, SIOCSIFFLAGS, &ifr) < 0) {
		printf("ioctl SIOCGIFFLAGS IFF_PROMISC\n");
		return -1;
	}

	//Recv a packt from net dev
	len = recvfrom(s, buf, BUF_SIZE, 0, (struct sockaddr *)&src, &src_len);
	if(len < 0) {
		printf("Error recvfrom\n");
		return -1;
	}

	//PART 2: recover data into skb
	//Allocate new skb with enough headroom
	skb = alloc_skb(SKB_DATA_ALIGN(data_len + LL_RESERVED_SPACE(ndev)), GFP_ATOMIC);
	if(!skb) {
		printf("skb allocation error. %s\n", strerror(errno));
		return -1;
	}

	//Reserve space for pkthdr
	skb_reserve(skb, LL_RESERVED_SPACE(ndev));

	//Increase data length of skb
	skb_put(skb, ndev->addr_len);

	//Copy data into skb data area
	memcpy(skb->data, src, dev->addr_len);

	//Print len and src addr of pkt
	printf("Received %d B from %s\n", len, inet_ntoa(src.sin_addr));
	
	printf("skb data. %s\n", skb->data);

	return 0;
}
