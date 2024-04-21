#include <stdio.h>
#include <pcap.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <net/ethernet.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <string.h>
#include <errno.h>


//Callback function to handle packets
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data);

void send_get_req(const char *url);

void parse_packet(const struct pcap_pkthdr *header, const u_char *pkt_data);

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <url>.\n", argv[0]);
		return 0;
	}
	pcap_t *adhandle; //A handle to the device
	char errbuf[PCAP_ERRBUF_SIZE];
	char *device;
	const char *url;
	/*Filter expression*/
	char packet_filter[] = "ip and tcp";
	/*Compiled filter*/
	struct bpf_program fcode;
	int res; //The result of pcap_next_ex
	
	//Find a device
	device = pcap_lookupdev(errbuf);
	if(device == NULL) {
		printf("Error finding device: %s\n", errbuf);
		return -1;
	}

	//Open device
	adhandle = pcap_open_live(device, 65536, 1, 1000, errbuf);
	if(adhandle == NULL) {
		printf("Error opening device: %s\n", errbuf); //pcap_geterr(adhandle));
		return -1;
	}

	//Compile the filter
	if(pcap_compile(adhandle, &fcode, packet_filter, 1, 0) < 0) {
		printf("Error compiling filter: %s\n", pcap_geterr(adhandle));
		return -1;
	}
	//Set filter
	if(pcap_setfilter(adhandle, &fcode) < 0) {
		printf("Error setting the filer: %s\n", pcap_geterr(adhandle));
		return -1;
	}

	//Set url of wsite
	url = argv[1];
	
	//Send GET req
	printf("Making a request to %s\n", url);
	send_get_req(url);

	printf("Capturing the packets\n");
	pcap_loop(adhandle, 0, packet_handler, NULL);

	//Close the device
	pcap_close(adhandle);

	return 0;
}

//Callback definition of packet handler
void packet_handler(u_char *param, const struct pcap_pkthdr *header, const u_char *pkt_data) {
	//parse_packet(header, pkt_data);

	//Get ethernet header
	struct ether_header *eth_hdr = (struct ether_header *) pkt_data;

	//Get source & dest MAC addr
	u_char *src_mac = eth_hdr->ether_shost;
	u_char *dst_mac = eth_hdr->ether_dhost;

	//Convert MAC addr to hex str
	char src_mac_str[18];
	char dst_mac_str[18];
	sprintf(src_mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", src_mac[0], src_mac[1], src_mac[2], src_mac[3], src_mac[4], src_mac[5]);
	sprintf(dst_mac_str, "%02x:%02x:%02x:%02x:%02x:%02x", dst_mac[0], dst_mac[1], dst_mac[2], dst_mac[3], dst_mac[4], dst_mac[5]);
	//Check if pkt is IP
	//File output
	FILE *outputFile;
	
	if(ntohs(eth_hdr->ether_type) == ETHERTYPE_IP) {
		//Open file
		outputFile = fopen("getreqdata.txt", "a");
		if(outputFile == NULL) {
			perror("Error opening file to save");
			return;
		}
		//Get IP hdr
		struct ip *ip_hdr = (struct ip *) (pkt_data + sizeof(struct ether_header));
		//Get src and dst IP addr
		struct in_addr src_ip = ip_hdr->ip_src;
		struct in_addr dst_ip = ip_hdr->ip_dst;

		//Convert ip addr to string
		char src_ip_str[INET_ADDRSTRLEN];
		char dst_ip_str[INET_ADDRSTRLEN];
		inet_ntop(AF_INET, &src_ip, src_ip_str, INET_ADDRSTRLEN);
		inet_ntop(AF_INET, &dst_ip, dst_ip_str, INET_ADDRSTRLEN);

		//Check if pkt is TCP
		if(ip_hdr->ip_p == IPPROTO_TCP) {
			//Get TCP hdr
			struct tcphdr *tcp_hdr = (struct tcphdr *)(pkt_data + sizeof(struct ether_header) + ip_hdr->ip_hl * 4);
			//Get src and dst port nums
			u_short src_port = ntohs(tcp_hdr->th_sport);
			u_short dst_port = ntohs(tcp_hdr->th_dport);
			//Check if pkt is http req
			if(src_port == 80 || dst_port == 80 || src_port == 443 || dst_port == 443) {
				//Get pkt payl
				u_char *payload = (u_char *)(pkt_data + sizeof(struct ether_header) + ip_hdr->ip_hl * 4 + tcp_hdr->doff * 4);
				//Get length of payl
				int payload_len = header->len - (sizeof(struct ether_header) + ip_hdr->ip_hl * 4 + tcp_hdr->doff * 4);
				//Check if payl is not empty
				if(payload_len > 0) {
					//Print MAC addrs, IP addrs, port nums and payl
					printf("Source MAC: %s\n", src_mac_str);
					fprintf(outputFile, "Source MAC: %s\n", src_mac_str);
					printf("Destination MAC: %s\n", dst_mac_str);
					fprintf(outputFile, "Destination MAC: %s\n", dst_mac_str);
					printf("Source IP: %s\n", src_ip_str);
					fprintf(outputFile, "Source IP Address: %s\n", src_ip_str);
					printf("Dest. IP: %s\n", dst_ip_str);
					fprintf(outputFile, "Destination IP Address: %s\n", dst_ip_str);
					printf("Source port: %hu\n", src_port);
					fprintf(outputFile, "Source port: %hu\n", src_port);
					printf("Dest. port: %hu\n", dst_port);
					fprintf(outputFile, "Destination port: %hu\n", dst_port);
					printf("Payload: %.*s\n", payload_len, payload);
					fprintf(outputFile, "Payload: %.*s\n", payload_len, payload);
					fflush(outputFile);
				}
			}
		}
		fclose(outputFile);
	}
}

void send_get_req(const char *url) {
	//Server structures
	int sockfd;
	struct sockaddr_in serv_addr;
	struct hostent *server;
	//host name of wsite
	char *host;
	//Path of wsite
	char *path;
	char request[1024]; /*A buffer for the req*/
	int portno; /*The port nr.*/
	//The nr. of B sent/received
	int n;

	//Parse url
	host = strtok(url, "/");
	path = strtok(NULL, "");
	//Set port no.
	portno = 443;

	sockfd = socket(AF_INET, SOCK_STREAM, 0);
	if(sockfd < 0) {
		printf("Error creating socket. %s\n", strerror(errno));
		return;
	}

	//Get host entity
	server = gethostbyname(host);
	if(server == NULL) {
		printf("Error getting host by name\n");
		return;
	}
	//Set the server addr fields
	bzero((char *) &serv_addr, sizeof(serv_addr));
	serv_addr.sin_family = AF_INET;
	bcopy((char *)server->h_addr, (char *)&serv_addr.sin_addr.s_addr, server->h_length);
	serv_addr.sin_port = htons(portno);

	//Connect to srvr
	if(connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
		printf("Error connecting to server: %s\n", strerror(errno));
		return;
	}

	//Form the req
	n = write(sockfd, request, strlen(request));
	if(n < 0) {
		printf("Error writing to socket: %s\n", strerror(errno));
		return;
	}
}
/*
//Function to parse packet and print info
void parse_packet(const struct pcap_pkthdr *header, const u_char *pkt_data) {
	//Get Eth header
	struct ether_header *eth_hdr = (struct ether_header *)pkt_data;

	//Get src and dst MAC addrs
	u_char *src_mac = eth_hdr->ether_shost;
	u_char *dst_mac = eth_hdr->ether_dhost;
	*/
