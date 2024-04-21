#include "packtscan.h"


//Function to process IP and TCP/ICMP headers
void process_packet(unsigned char *buffer, int size) {
	//Extract packet content (IP header, TCO header, etc.)
	struct iphdr *ip_header = (struct iphdr *)(buffer);
	struct tcphdr *tcp_header = (struct tcphdr *)(buffer + sizeof(struct iphdr));
	//Analyze headers and payload to identify req type and extract data
	int ipversion = ip_header->version;
	int ipihl = ip_header->ihl;
	int ipproto = ip_header->protocol;
	int iplen = ip_header->tot_len;
	//int ipvhlen = ip_header->verhdrlen;
	uint32_t ipsrcaddr = ip_header->saddr;
	uint32_t ipdstaddr = ip_header->daddr;
	char ipsrc[40];
	char ipdst[40];
	int ip_header_size = ipihl * 4;
	int tcp_header_size = tcp_header->doff * 4;
	int payload_size = ntohs(ip_header->tot_len) - ip_header_size - tcp_header_size;
	inet_ntop(AF_INET, &ipsrcaddr, ipsrc, INET_ADDRSTRLEN);
	inet_ntop(AF_INET, &ipdstaddr, ipdst, INET_ADDRSTRLEN);
	if(ipversion == 4) {
		//IPv4 hdr
		if(ipihl > 5) {
			printf("Protocol has a ipihl > 5, %d\n", ipihl);
		}
	}
	printf("IP version: %d\n", ipversion);
	printf("IP IHL: %d\n", ipihl);
	printf("IP protocol: %d (%s)\n", ipproto, ipproto == 6 ? "TCP" : "ICMP");
	printf("IP length: %d\n", iplen);
	printf("IP source address: %s\n", ipsrc);
	printf("IP destination address: %s\n", ipdst);
	
	//File to save payload
	FILE *file = fopen("./tcp_payload.txt", "w");		
	if(file == NULL) {
		perror("Unable to open file\n");
		return;
	}

	//Print payload
	printf("TCP payload:\n");

	for(int i = 0; i < payload_size; i++) {
		printf("%02x ", buffer[ip_header_size + tcp_header_size + i]);
		//newline for readability
		if((i + 1) % 16 == 0) {
			printf("\n");
		}
		//Save to .txt the payload
		//Open file in write mode
		char c = buffer[ip_header_size + tcp_header_size + i];
		if(c <= 127 || ((c & 0xC0) != 0x80))
			fputc(c, file);	
		else
			printf("Buffer is not ASCII or UTF-8 coded\n");
	}
	fflush(file);
	fclose(file);
}

	//Callback function to process Ethernet headers
void ethHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet) {
	//Cast packet to Eth header
	struct ether_header *eth_header = (struct ether_header *)packet;
	printf("Source MAC address: %.2x:%.2x:%.2x:%.2x%.2x:%.2x\n",
			eth_header->ether_shost[0],
			eth_header->ether_shost[1],
			eth_header->ether_shost[2],
			eth_header->ether_shost[3],
			eth_header->ether_shost[4],
			eth_header->ether_shost[5]);
	printf("Destination MAC address: %.2x:%.2x:%.2x:%.2x%.2x:%.2x\n",
			eth_header->ether_dhost[0],
			eth_header->ether_dhost[1],
			eth_header->ether_dhost[2],
			eth_header->ether_dhost[3],
			eth_header->ether_dhost[4],
			eth_header->ether_dhost[5]);
	printf("Type of Ethernet header: %.2x\n", eth_header->ether_type);
	int ethsz = sizeof(eth_header);
	printf("Ethernet header size: %d\n", ethsz);
	return;// ethsz;
}

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <interface> [<req-type>]\n", argv[0]);
		return -1;
	}
	in_addr_t routip;
	const char *ifname;
	ifname = (char *)malloc(20);
	strcpy(ifname, argv[1]);
	struct sockaddr_in routaddr;
	//routaddr = (struct sockaddr_in *)malloc(sizeof(struct sockaddr_in));
	//routaddr.sin_family = AF_INET;
	//routaddr.sin_port = 0;//SERV_PORT;
	//Inet_pton(AF_INET, ipaddr, &routaddr.sin_addr);
	//routaddr->sin_addr = routip;

	//Socket to handle traffic in interface
	int raw_socket = Socket(AF_INET, SOCK_RAW, IPPROTO_TCP);

	unsigned char buffer[BUFSIZE];
	memset(buffer, 0, BUFSIZE);

	//int ifidx = if_nametoindex(ifname);
	//Try to avoid bind error
	int optval = 1;

	pcap_t *handle;
	char errbuf[PCAP_ERRBUF_SIZE];

	if(setsockopt(raw_socket, SOL_SOCKET, SO_BINDTODEVICE, ifname, sizeof(ifname)) < 0) {
		perror("Setsockopt failed");
		exit(EXIT_FAILURE);
	}

	//Bind(raw_socket, (struct sockaddr *) &routaddr, sizeof(routaddr));

	//Connect(raw_socket, (struct sockaddr *) &routaddr, sizeof(routaddr));

	while(1) {
		int data_size = recv(raw_socket, buffer, sizeof(buffer), 0);//MSG_DONTWAIT);
		if(data_size < 0) {
			printf("Error recv. %s\n", strerror(errno));
			return -1;
		} else if(data_size == 0) {
			//No more data to read
			break;
		}
		printf("Recvd %d bytes\n", sizeof(buffer));
		
		//Open network if for packet capt
		handle = pcap_open_live(ifname, BUFSIZE, 1, 0, errbuf); 
		pcap_loop(handle, 60, ethHandler, NULL);	
		process_packet(buffer, data_size);
	}
	Close(raw_socket);
	//free(routaddr);
	free(ifname);
	return 0;
}
