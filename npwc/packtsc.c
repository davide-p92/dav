#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pcap.h>
#include <net/if.h>

void packtHandlr(unsigned char *userData, const struct pcap_pkthdr *pkthdr, const unsigned char *packet) {
	//printf("Rec packet: \n");
	
}

int main(int argc, char **argv) {
	
	if(argc != 2) {
		printf("usage. %s <if_name>\n", argv[0]);
		return -1;
	}

	char errbuf[PCAP_ERRBUF_SIZE];
	
	char dev[IFNAMSIZ]; //e.g. enp0s3
	strcpy(dev, argv[1]);

	pcap_t *handle = pcap_open_live(dev, BUFSIZ, 1, 1000, errbuf);
	if(handle == NULL)  {
		printf("Error opening dev %s: %s\n", dev, errbuf);
		return -1;
	}

	//Compile and set filter to capture TCP, UDP, ICMP, DHCP
	struct bpf_program fp;
	const char filter_exp[] = "tcp or udp or icmp or arp or ip6 or llc i";
	if(pcap_compile(handle, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
		printf("Error parsing filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return -1;
	}

	if(pcap_setfilter(handle, &fp) == -1) {
		printf("Error setting filter %s: %s\n", filter_exp, pcap_geterr(handle));
		return -1;
	}

	//Cap packts
	pcap_loop(handle, -1, packtHandlr, NULL);
	/*pcap_dumper_t *pd;
	pcap_t *p;
	int rc = 0;
	pd = pcap_dump_open(p, "/t
*/
	pcap_close(handle);
	return 0;
}

