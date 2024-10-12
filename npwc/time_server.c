#include <sys/types.h>
#include<sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>

#define ISVALIDSOCKET(s) ((s) >= 0)
#define CLOSESOCKET(s) close(s)
#define SOCKET int
#define GETSOCKETERRNO() (errno)

#include <stdio.h>
#include <string.h>
#include <time.h>

int main() {
	struct addrinfo hints;
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE; /* wildcard, listen on any available
					interface */

	struct addrinfo *bind_addr;
	getaddrinfo(0, "8080", &hints, &bind_addr); //first argum: suitable for bind()
	
	//sock creat
	SOCKET socket_listen;
	socket_listen = socket(bind_addr->ai_family, bind_addr->ai_socktype, bind_addr->ai_protocol);
	
	if(!ISVALIDSOCKET(socket_listen)) {
		printf("Error socket: %s\n", strerror(errno));
		return -1;
	}
	printf("binding sock to local address...\n");
	if(bind(socket_listen, bind_addr->ai_addr, bind_addr->ai_addrlen) < 0) {
		printf("bind error: %s\n", strerror(errno));
		return -1;
	}
	//free bind_addr
	freeaddrinfo(bind_addr);

	printf("Listening...\n");
	if(listen(socket_listen, 10) < 0) {
		printf("Error listen: %s\n", strerror(errno));
		return -1;
	}
	printf("Waiting for conns...\n");
	struct sockaddr_storage client_addr;
	socklen_t client_len = sizeof(client_addr);
	//Block program until conn is made
	SOCKET socket_client = accept(socket_listen, (struct sockaddr *)&client_addr, &client_len);
	if(!ISVALIDSOCKET(socket_client)) {
		printf("error accept: %s\n", strerror(errno));
		return 1;
	}
	
	char address_buffer[100];
	//get and print client address
	getnameinfo((struct sockaddr*)&client_addr, client_len, address_buffer, sizeof(address_buffer), 0, 0, NI_NUMERICHOST);
	printf("%s\n", address_buffer);

	printf("Reading req...\n");
	char req[1024];
	int bytes_received = recv(socket_client, req, 1024, 0);
	printf("Rec %d bytes\n", bytes_received);

	//We can now echo resp
	const char *resp = "HTTP/1.1 200 OK\r\n"
		"Connection: close \r\n"
		"Content-Type: text/plain\r\n"
		"Local time is: ";
	int bytes_sent = send(socket_client, resp, strlen(resp), 0);
	printf("Sent %d bytes of %d\n", bytes_sent, (int)strlen(resp));

	time_t timer;
	time(&timer);
	char *time_msg = ctime(&timer);
	bytes_sent = send(socket_client, time_msg, strlen(time_msg), 0);
	printf("Sent %d of %d bytes\n", bytes_sent, (int)strlen(time_msg));
	printf("Closing conn...\n");
	CLOSESOCKET(socket_client);

	printf("Closing listening socket...\n");
	CLOSESOCKET(socket_listen);

	return 0;
}
