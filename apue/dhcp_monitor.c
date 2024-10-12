#include "apue.h"
#include <netinet/udp.h>
#include <netinet/ip.h>
#include <linux/if_ether.h>
#include <linux/if_packet.h>
#include <linux/if_arp.h>

int main() {
	int sock_raw;
	sock_raw = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
	if(sock_raw < 0) {
		perror("Socket Error");
		return 1;
	}

	unsigned char *buffer = (unsigned char *)malloc(65536);
	while(1) {
		int data_size = recvfrom(sock_raw, buffer, 65536, 0, NULL, NULL);
		if(data_size < 0) {
			perror("Recvfrom error");
			return 1;
		}

		//Parse Ethernet header, IP header, UDP header
		struct ethhdr *eth = (struct ethhdr *)buffer;
		if(ntohs(eth->h_proto) == ETH_P_IP) {
			struct iphdr *iph = (struct iphdr *)(buffer + sizeof(struct ethhdr));
			if(iph->protocol == 17) { //UDP protocol
				struct udphdr *udph = (struct udphdr *)(buffer + iph->ihl * 4 + sizeof(struct ethhdr));
				if(ntohs(udph->source) == 67 || ntohs(udph->source) == 68) {
					printf("DHCP Packet Detected by karola raket: ");
					printf("Source IP: %s", inet_ntoa(*(struct in_addr *)&iph->saddr));
					printf("Destination IP: %s\n", inet_ntoa(*(struct in_addr *)&iph->daddr));

				}
			}
		}
	}
	close(sock_raw);
	return 0;
}
