#include "unp.h"
#include <netinet/ip_icmp.h>

#define PACKET_SIZE 64
#define MAX_ATTEMPTS 3
#define TIMEOUT 1

unsigned short calculate_checksum(unsigned short *addr, int len) {
	unsigned long sum = 0;
	while(len > 1) {
		sum += *addr++;
		len -= 2;
	}
	if(len == 1) {
		sum += *((unsigned char *)addr);
	}
	sum = (sum >> 16) + (sum & 0xffff);
	sum += (sum >> 16);
	return (unsigned short)(~sum);
}

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("Usage: %s <Dest. IP address> <Source IP address>\n", argv[0]);
		return -1;
	}
	int sockfd;
	char packet[PACKET_SIZE];
	struct sockaddr_in dest;
	struct iphdr *ip = (struct iphdr *)packet;
	struct icmphdr *icmp = (struct icmphdr *)(packet + sizeof(struct iphdr));
	
	//Create socket
	sockfd = Socket(AF_INET, SOCK_RAW, IPPROTO_ICMP);

	dest.sin_family = AF_INET;
	Inet_pton(AF_INET, argv[1], &dest.sin_addr);

	memset(packet, 0, PACKET_SIZE);

	//IP hdr
	ip->ihl = 5;
	ip->version = 4;
	ip->tos = 0;
	ip->tot_len = sizeof(struct iphdr) + sizeof(struct icmphdr);
	ip->id = htons(54321);
	ip->ttl = 64;
	ip->protocol = IPPROTO_ICMP;
	ip->saddr = inet_addr(argv[2]);
	ip->daddr = dest.sin_addr.s_addr;
	ip->check = calculate_checksum((unsigned short *)packet, ip->tot_len);

	//ICMP hdr
	icmp->type = ICMP_ECHO;
	icmp->code = 0;
	icmp->un.echo.id = getpid();
	icmp->un.echo.sequence = 0;
	icmp->checksum = 0;
	icmp->checksum = ((unsigned short *)icmp, sizeof(struct icmphdr));

	//Send packets to probe for devices
	int attempts;
	for(attempts = 0; attempts < MAX_ATTEMPTS; attempts++) {
		if(sendto(sockfd, packet, ip->tot_len, 0, (struct sockaddr *)&dest, sizeof(dest)) <= 0) {
			perror("Sendto error");
			exit(EXIT_FAILURE);
		}
		sleep(TIMEOUT);
		uint8_t type, code;
		int chcksm, gw;
		printf("Type: %d\nCode:%d\n", type, code);
		printf("\nChecksum: %d\nGateway:%d\n", chcksm, gw);

	}
	Close(sockfd);
	return 0;
}
