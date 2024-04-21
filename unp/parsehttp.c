#include "parsehttp.h"

void parse_http_req(const char *req) {
	//Find end of request line
	const char *endofreq = strstr(req, "\r\n");
	if(endofreq == NULL) {
		printf("Invalid HTTP request\n");
		return;
	}

	//Extract request line (method, URL, HTTP version)
	char reqline[LINESIZE];
	strncpy(reqline, req, endofreq - req);
	reqline[endofreq - req] = '\0';

	printf("Reqline: %s\n", reqline);
	//Tokenize request line to extract method and URL
	char *method = strtok(reqline, " "); /*extract method*/
	char *url = strtok(NULL, " "); /*extract URL*/
	char *httpv = strtok(NULL, " "); /*extract HTTP version*/

	if(method == NULL || url == NULL) {
		printf("Invalid HTTP request\n");
		return;
	}

	printf("Method: %s\n", method);
	printf("URL: %s\n", url);
	printf("HTTP version: %s\n", httpv);
}

void info_callback(const SSL *ssl, int type, int val) {
	if(type == SSL_CB_HANDSHAKE_START) {
		printf("SSL handshake started...\n");
		const char *vers = SSL_get_version(ssl);
		printf("Protocol version: %s\n", vers); /*print protocol version*/
		//Get cypher suite information
		const SSL_CIPHER *cipher = SSL_get_current_cipher(ssl);
		if(cipher) {
			printf("Cipher: %s\n", SSL_CIPHER_get_name(cipher));
		}

	} else if(type == SSL_CB_HANDSHAKE_DONE) {
		printf("SSL handshake completed successfully.\n");
		//Log handshake completion status
	} else if(type == SSL_CB_ALERT) {
		int alert_type = SSL_alert_type_string_long(val);
		const char *alert_msg = SSL_alert_desc_string_long(val);
		printf("SSL alert: %d %s\n", alert_type, alert_msg);
	}
}
