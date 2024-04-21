#include "unp.h"
#include <netinet/ether.h>
#include <netinet/ip.h>
#include <netinet/tcp.h>
#include <net/if.h>
#include <pcap/pcap.h>

#define BUFSIZE 65536

//Function to process TCP and IP headers
void process_packet(unsigned char *buffer, int size);

//Callback function to process Eth headers
void ethHandler(u_char *userData, const struct pcap_pkthdr *pkthdr, const u_char *packet);
