#include "unp.h"

void dg_cli(FILE *fp, int sockfd, const struct sockaddr *pservaddr, socklen_t servlen) {
	int n;
	char sendline[MAXLINE], recvline[MAXLINE + 1];
	socklen_t len;

	struct sockaddr *preply_addr;
	struct hostent *hent, *hent_dns;
	preply_addr = malloc(servlen);
	if(preply_addr < 0) {
		printf("error malloc. %s\n", strerror(errno));
		return;
	}

	while(Fgets(sendline, MAXLINE, fp) != NULL) {
		Sendto(sockfd, sendline, strlen(sendline), 0, pservaddr, servlen);
		len = servlen;
		n = Recvfrom(sockfd, recvline, MAXLINE, 0, preply_addr, &len); //NULL, NULL);
		//Verify reply received has same dimensions as allocated
		if(len != servlen || memcmp(pservaddr, preply_addr, len) != 0) {
			printf("reply from %s (ignored)\n", sock_ntop(preply_addr, len));
			continue;
		}

		/*Ex 11.6: query DNS to find all destination host IP addresses*/
		hent = gethostbyaddr(preply_addr, sizeof(preply_addr), AF_UNSPEC);
		printf("host entry: %s\n", hent->h_name);
		//Query DNS with found IP addresses
		hent_dns = gethostbyname(hent->h_name);
		for(int i = 0; i < sizeof(hent_dns->h_addr_list); i++)
			printf("DNS entry: %s\n", hent_dns->h_addr_list[i]);

		recvline[n] = 0; /*null-termin*/
		Fputs(recvline, stdout);
	}
}
