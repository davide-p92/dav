#include "unp.h"
#include <memory.h>
#include <sys/types.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <openssl/rsa.h>
#include <openssl/crypto.h>
#include <openssl/x509.h>
#include <openssl/pem.h>
#include <openssl/ssl.h>
#include <openssl/err.h>

#define HOME "./" //where certs and keys are stored
#define CERTF HOME "server.crt" /*server certificate file*/
#define KEYF HOME "server.key" /*server private key file*/
#define CACERT HOME "ca.crt" /*the certificate authority file*/

#define CHK_NULL(x)
if ((x) == NULL) exit(1);
#define CHK_ERR(err, s) 
if ((err) == -1) {
	perror(s);
	exit(1);
}
#define CHK_SSL(err)
if ((err) == -1) {
	ERR_print_errors_fp(stderr);
	exit(2);
}


int main(void) {
	int err;
	int listen_sd;
	int sd;
	struct sockaddr_in sa_serv;
	struct sockaddr_in sa_cli;
	size_t client_len;
	SSL_CTX *ctx;
	SSL *ssl;
	X509 *client_cert;
	char *str;
	char buf[4096];
	SSL_METHOD *meth;

	//Load error strs and SSL algorithms
	SSL_load_error_strings();
	SSLeay_add_ssl_algorithms();
	meth = TLSv1_2_server_method(); /*Use TLS 1.2 as the server method*/
	ctx = SSL_CTX_new(meth);
	//Create a new SSL context
	CHK_NULL(ctx);

	//Load the server certificate and private key
	if( SSL_CTX_use_certificate_file(ctx, CERTF, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		exit(3);
	}
	if( SSL_CTX_use_PrivateKey_file(ctx, KEYF, SSL_FILETYPE_PEM) <= 0 ) {
		ERR_print_errors_fp(stderr);
		exit(4);
	}

	/*Check if the server certificate and private key match*/
	if(!SSL_CTX_check_private_key(ctx)) {
		fprintf(stderr, "Private key does not match the certificate public key \n");
		exit(5);
	}

	/*Load the certificate authority file and set the verify mode*/
	if(!SSL_CTX_load_verify_locations(ctx, CACERT, NULL)) {
		ERR_print_errors_fp(stderr);
		exit(6);
	}
	SSL_CTX_set_verify(ctx, SSL_VERIFY_PEER, NULL); //Request the client certificate
	/*Set verification depth to 1*/
	SSL_CTX_set_verify_depth(ctx, 1);

	/*Create a TCP socket and bind it to a port*/
	listen_sd = socket(AF_INET, SOCK_STREAM, 0);
	CHK_ERR(listen_sd, "socket");
	memset(&sa_serv, 0x00, sizeof(sa_serv));
	sa_serv.sin_family = AF_INET;
	sa_serv.sin_addr.s_addr = INADDR_ANY;
	sa_serv.sin_port = htons(1111);
	err = bind(listen_sd, (struct sockaddr *)&sa_serv, sizeof(sa_serv));
	CHK_ERR(err, "bind");

	/*Listen for incoming connections*/
	err = listen(listen_sd, 5);
	CHK_ERR(err, "listen");

	/*Accept a connection from a client*/
	client_len = sizeof(sa_cli);
	sd = accept(listen_sd, (struct sockaddr *)&sa_cli, &client_len);
	CHK_ERR(sd, "accept");
	close(listen_sd); /*close listening socket*/

	/*Create a new SSL obj and associate it with socket
	 */
	ssl = SSL_new(ctx);
	CHK_NULL(ssl);
	SSL_set_fd(ssl, sd);

	/*Perform SSL handshake
	 */
	err = SSL_accept(ssl);
	CHK_SSL(err);

	/*Print the SSL cipher used
	 */
	printf("SSL connection using %s\n", SSL_get_cipher(ssl));
	
	/*Get the client certificate and print some information about it
	 */
	client_cert = SSL_get_peer_certificate(ssl);
	if(client_cert != NULL) {
		printf("Client certificate:\n"),
		str = X509_NAME_oneline(X509_get_subject_name(client_cert), 0, 0);
		CHK_NULL(str);
		printf("\t subject: %s\n", str);
		OPENSSL_free(str);
		X509_free(client_cert);
	} else {
		printf("Client does not have certificate.\n");
	}

	/* Read a message from the client */
	err = SSL_read(ssl, buf, sizeof(buf) - 1);
	CHK_SSL(err);
	buf[err] = '\0';
	printf("Got %d characters: %s.\n", err, buf);

	/*Write a message to the client
	 */
	err = SSL_write(ssl, "I hear you.", strlen("I hear you."));
	CHK_SSL(err);

	/*Close the SSL connection and free the SSL objs
	 */
	SSL_shutdown(ssl);
	close(sd);
	SSL_free(ssl);
	SSL_CTX_free(ctx);
	return 0;
}
	
