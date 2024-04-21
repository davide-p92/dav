#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libwebsockets.h>
#include <curl/curl.h>
#include <cjson/cJSON.h>
#include <pthread.h>

//Structure holding context and wsocket
struct client {
	struct lws_context *context; /*Context for web socket*/
	struct lws *wsi; /*Wsocket instance*/
};

//Callback func handling wsocket events
static int callback_client(struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len);

struct response_data {
	char *data; /*resp data*/
	size_t size; /*size of data*/
};

static size_t write_response_data(void *contents, size_t size, size_t nmemb, void *userp);

void parse_data(char *in);

void send_message(struct lws *wsi);

void run_client(char *url);

int *getpageId(char *url);

void process_data(cJSON *id, cJSON *method, cJSON *params);

//A function creating and destroying a mutex context
void create_and_destroy_context(struct lws_context *context);
