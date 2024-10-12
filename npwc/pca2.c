#include <stdio.h>
#include <string.h>
#include <pcap/pcap.h>
#include <net/if.h>


void packtHndlr(u_char *usrData, const struct pcap_pkthdr *pkthdr, const u_char *packt) {
	printf("rec packet: %d\n", pkthdr->caplen);
}	

int main(int argc, char **argv) {
	if(argc != 3) {
		printf("usage: %s <ifname> <outfilename>\n", argv[0]);
		return -1;
	}
	char ifname[IFNAMSIZ];
	char outf[128];
	strcpy(ifname, argv[1]);
	strcpy(outf, argv[2]);
	pcap_t *p;
	struct pcap_pkthdr pkthdr;
	pcap_dumper_t *usr; //first param of pcap_dump
	
	/*1. open live */
	char errbuf[PCAP_ERRBUF_SIZE];
	if((p = pcap_open_live(ifname, BUFSIZ, 1, 1000, errbuf)) == NULL) {
		printf("Error pcap_open_live on if %s : %s\n", ifname, errbuf);
		return -1;
	}
	pcap_set_snaplen(p, 65535);//suffices for most networks
	/*2 set up filter */
	struct bpf_program fp;
	const char filter_exp[] = "tcp or udp or icmp or ip6 or arp";
	if(pcap_compile(p, &fp, filter_exp, 0, PCAP_NETMASK_UNKNOWN) == -1) {
		printf("Error pcap_compile on if %s : %s\n", ifname, pcap_geterr(p));
		return -1;
	}
	if(pcap_setfilter(p, &fp) == -1) {
		printf("Error set filter %s: %s\n", filter_exp, pcap_geterr(p));
		return -1;
	}
	pcap_dispatch(p, 100, packtHndlr, NULL);

	/*2. set up savefile */
	if((usr = pcap_dump_open(p, outf)) == NULL) {
		printf("Error pcap_dump_open : %s\n", pcap_geterr(p));
		return -1;
	}
	/*3. create savefile handle*/
	/*if(*/pcap_dump((u_char *)&usr, &pkthdr, NULL);// == NULL) {
	//	printf("Error pcap_dump\n");
	//	return -1;
	//}
	return 0;
}
	
