#ifndef PARSEHTTP_H
#include "parsehttp.h"
#endif
//#include <openssl/ssl.h>
//#include <openssl/err.h>

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <url>\n", argv[0]);
		return -1;
	}
	SSL_library_init();
	SSL_CTX *ctx = SSL_CTX_new(TLS_client_method());
	char wurl[50];
	struct sockaddr_in servaddr;
	//sockaddr_in to get IP addr
	struct sockaddr_in *servip;
	struct addrinfo hints, *res; /*store info on website*/
	int sockfd;
	//wurl = (char *)malloc(50);
	if(wurl != NULL) {
		/*if(strstr("http", wurl) == NULL) {
			sprintf((char *)wurl, "https://%s\0", wurl);
		}*/
		strcpy(wurl, argv[1]);
		printf("URL: %s\n", wurl);
	} else {
		printf("Error malloc. %s\n", strerror(errno));
		return -1;
	}
	
	//Set up hints for getaddrinfo
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC; //IPv4 or IPv6
	hints.ai_socktype = SOCK_STREAM;
	int status = getaddrinfo(wurl, "443", &hints, &res);
	if(status != 0) {
		printf("Error getaddrinfo: %s\n", strerror(errno));
		//free(wurl);
		return -1;
	}
	
	//Create socket and bind to web server
	if(res->ai_protocol == IPPROTO_TCP)
		printf("Protocol: TCP\n");
	else if(res->ai_protocol == IPPROTO_UDP)
		printf("Protocol: UDP\n");
	else
		printf("Unknown protocol\n");
		
	printf("Protocol: %d\n", res->ai_protocol);
	sockfd = Socket(res->ai_family, res->ai_socktype, res->ai_protocol);
	printf("Sockfd: %d\n", sockfd);	
	//Wrap socket in SSL/TLS connect
	SSL *ssl = SSL_new(ctx);
	if(ssl == NULL) {
		printf("Error SSL_new: %s\n", strerror(errno));
		return -1;
	}
	//Prepare servaddr to connect
	memset(&servaddr, 0, sizeof(servaddr));
	char servip_str[INET_ADDRSTRLEN];
	servip = (struct sockaddr_in *)res->ai_addr;
	//Buffer to hold IP address string
	if(inet_ntop(AF_INET, &(servip->sin_addr), servip_str, INET_ADDRSTRLEN) == NULL) {
		perror("inet_ntop");
		return -1;
	}
	servaddr.sin_family = res->ai_family;
	servaddr.sin_port = htons(443);
	//Convert the IP address back to binary
	Inet_pton(AF_INET, servip_str, &servaddr.sin_addr);
	Connect(sockfd, (struct sockaddr *) &servaddr, sizeof(servaddr));
	SSL_set_fd(ssl, sockfd);

	//DEBUG: Callback for SSL/TLS handshake
	SSL_CTX_set_info_callback(ctx, info_callback);
	//Perform SSL/TLS handshake
	if(SSL_connect(ssl) != 1) {
		printf("Error SSL_connect.\n");
		ERR_print_errors_fp(stderr);
		SSL_free(ssl);
		SSL_CTX_free(ctx);
		//free(wurl);
	}
	//Set socket options
	int optval = 1;
	printf("Optval: %d\n", optval);
	if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval)) == -1) {
		printf("Error setsockopt. %s\n", strerror(errno));
		//free(wurl);
		return -1;
	}

	//Read decrypted data from SSL connection
	char buffer[BUFSIZE];
	SSL_set_mode(ssl, SSL_MODE_ENABLE_PARTIAL_WRITE);
	int bytes_read;
	int retry_count = 0;
	while(1) {
		bytes_read = SSL_read(ssl, buffer, sizeof(buffer));
		if(bytes_read > 0) {
			printf("%d bytes read\n", bytes_read);
			fwrite(buffer, 1, bytes_read, stdout);
		} else if(bytes_read == 0) {
			printf("No data read or connection closed by peer\n");
			int ssl_error = SSL_get_error(ssl, bytes_read);
			if(ssl_error != SSL_ERROR_NONE) {
				fprintf(stderr, "SSL error: %d\n", ssl_error);
				ERR_print_errors_fp(stderr);
			}
			break;
		} else {
			printf("Error SSL_read\n");
			int ssl_err = SSL_get_error(ssl, bytes_read);
			if(ssl_err == SSL_ERROR_WANT_READ || ssl_err == SSL_ERROR_WANT_WRITE) {
				//Retry read operation after a certain time
				printf("Retrying read after %d seconds...\n", RETRY_INTERVAL_SECONDS);
				if(++retry_count > MAX_RETRIES) {
					printf("Exceeded maximum retry attempts. Aborting.\n");
					break;
				}
				sleep(RETRY_INTERVAL_SECONDS);
			} else {
				printf("SSL generic read error. Code: %d\n", ssl_err);
				break;
			}
		}

		//DBG request
		printf("Buffer: %s\n", buffer);
		//Parse HTTP request
		parse_http_req(buffer);
	
	}
	freeaddrinfo(res);
	Close(sockfd);
	return 0;
}
