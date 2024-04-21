#ifndef PARSEHTTP_H
#define PARSEHTTP_H
#include "unp.h"
#include <openssl/ssl.h>
#include <openssl/err.h>

#define LINESIZE 256
#define BUFSIZE 4096
#define RETRY_INTERVAL_SECONDS 15
#define MAX_RETRIES 10

void parse_http_req(const char *req);
void info_callback(const SSL *ssl, int type, int val);

#endif
