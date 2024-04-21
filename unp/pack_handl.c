#include <stdio.h>
#include <stdlib.h>
#include <pcap.h>
#include <ctype.h>


void packetHandler(unsigned char *userData, const struct pcap_pkthdr *pkthdr, const 
		unsigned char *packet) {
	unsigned int payload_offset = 14; //Assuming Ethernet frame without VLAN tags
	//display packet info
	printf("Packet captured: Length: %d.\n", pkthdr->len);
	//Can manipulate here if needed
	unsigned int payload_length = pkthdr->len - payload_offset;
	//display payload content as hex vals:
	printf("Packet payload (hex): \n");
	for(unsigned int k = payload_offset; k < pkthdr->len; ++k) {
		printf("%02x ", packet[k]);
	}
	printf("\n");
	//display payload as ASCII char
	printf("Packet payload (ASCII):\n");
	for(unsigned int k = payload_offset; k < pkthdr->len; ++k) {
		if(isprint(packet[k])) {
			printf("%c", packet[k]);
		} else {
			printf(".");
		}
	}
	printf("\n");
}


int main(int argc, char **argv) {
	char errbuf[PCAP_ERRBUF_SIZE];
	pcap_t *handle;
	if(argc != 2) {
		fprintf(stderr, "Usage: %s <device to analyze>.\n", argv[0]);
		return(EXIT_FAILURE);
	}
	//Open network interface for capturing packets
	handle = pcap_open_live(argv[1], BUFSIZ, 1, 1000, errbuf);
	
	if(handle == NULL) {
		fprintf(stderr, "Couldn't open devide: %s\n", errbuf);
		return(EXIT_FAILURE);
	}

	//Set filter to capture specific packets
	struct bpf_program fp;
	char filter_exp[] = "icmp"; //Capture ICMP packts, change if needed
	if(pcap_compile(handle, &fp, "", 0, PCAP_NETMASK_UNKNOWN) == -1) {
		fprintf(stderr, "Couldn't parse filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(EXIT_FAILURE);
	}
	if(pcap_setfilter(handle, &fp) == -1) {
		fprintf(stderr, "Couldn't install filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return(EXIT_FAILURE);
	}

	//start capturing packets
	pcap_loop(handle, -1, packetHandler, NULL);
	//close capture handle
	pcap_close(handle);

	return 0;
}
