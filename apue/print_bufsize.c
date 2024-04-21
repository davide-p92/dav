#include "unp.h"
//#include <netinet/sctp.h>

void printBuffSz(int socketType, int protocol, const char *typeName) {
	int sockfd;
	/*if(protocol == IPPROTO_SCTP) {
		sockfd = Socket(socketType, SOCK_SEQPACKET, protocol);
	} else {
		sockfd = Socket(socketType, SOCK_STREAM, protocol);
	}*/
	sockfd = Socket(socketType, SOCK_STREAM, protocol);
	int sendBufferSize, recvBufferSize;
	socklen_t optlen = sizeof(sendBufferSize);
	//Retrieve send buffer size
	if(getsockopt(sockfd, SOL_SOCKET, SO_SNDBUF, &sendBufferSize, &optlen) == -1) {
		perror("getsockopt");
		return -1;
	}
	optlen = sizeof(recvBufferSize);

	//Retrieve receive buffer size
	if(getsockopt(sockfd, SOL_SOCKET, SO_RCVBUF, &recvBufferSize, &optlen) == -1) {
		perror("getsockopt");
		return -1;
	}
	
	printf("%s (Protocol %d) - Send Buffer Size: %d, Receive Buffer Size: %d\n", typeName, protocol, sendBufferSize, recvBufferSize);

	Close(sockfd);
}

int main() {
	//Print default buffer sizes for TCP, UDP and SCTP
	printBuffSz(AF_INET, IPPROTO_TCP, "TCP");
	printBuffSz(AF_INET, 0, "UDP");
	//printBuffSz(AF_INET, IPPROTO_SCTP, "SCTP");
	return 0;
}
