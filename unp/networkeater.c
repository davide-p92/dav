#include "networkeater.h"

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

//int *get_pageId(char *url);

int main(int argc, char **argv) {
	if(argc != 2) {
		printf("Usage: %s <url>.\n", argv[0]);
		return -1;
	}
	char *url; /*wsite url*/
	url = argv[1];

	//Run the client
	run_client(url);

	return 0;
}

//A callback function that handles the wsocket events
static int callback_client (struct lws *wsi, enum lws_callback_reasons reason, void *user, void *in, size_t len) {
	struct client *client = (struct client *) user; /*the client struct*/
	char *message; /*Buff for msg*/
	int n; /*nr B recvd*/

	switch(reason) {
		case LWS_CALLBACK_CLIENT_ESTABLISHED:
		//Wsock is establishd
			printf("Web socket established.\n");
			//Allocate memory for the message
			message = (char *)malloc(LWS_PRE + 2560);

			//Form msg to enable ntwork domain
			sprintf(message + LWS_PRE, "{\"id\": 1, \"method\":\"Network.enable\"}");
			//Write msg to wsocket
			n = lws_write(wsi, (unsigned char *) message + LWS_PRE, strlen(message + LWS_PRE), LWS_WRITE_TEXT);
			if(n < 0) {
				printf("Error writing to web socket\n");
				return -1;
			}

			//Free mem
			free(message);

			break;
		case LWS_CALLBACK_CLIENT_RECEIVE:
		//The wsocket receives data
			printf("Web socket recvd data: %s\n", (char *) in);
			//TODO: parse data
			parse_data((char *)in);

			break;
		case LWS_CALLBACK_CLIENT_WRITEABLE:
		//The wsock is writeable
			printf("Web socket is writeable");
			//Send message
			send_message(wsi);

			break;
		case LWS_CALLBACK_CLIENT_CLOSED:
		//The wsocket is closed
			printf("Web socket closed\n");
		
			//Destroy the context
			//lock();
			create_and_destroy_context(client->context);
			//lws_context_destroy(client->context);
			//unlock();

			break;

		default:
			break;
	}
	
	return 0;
}

//Function creating and running cli
void run_client(char *url) {
	struct lws_context_creation_info info; /*struct holding context creation info*/
	struct lws_client_connect_info i; /*struct holding client conn info*/
	struct lws_protocols protocols [] = {
		//Array of protocols supported by cl
		{"client-protocol", callback_client, sizeof(struct client), 0},
		{NULL, NULL, 0, 0}
	};
	struct client *client; /*cli struct*/
	lws_ctx_t context;
	int port; /*port nr of browser*/
	char *host; /*host name of brows*/
	char *path; /*path of brows*/

	//Set port nr to 9222 (default for DevTools)
	port = 9222;

	//Set hostn
	host = "localhost";

	//Get page id
	path = (char *)getpageId(url);
	//path = (char *)(intptr_t)intValue;
	if(path < 0) {
		printf("Error getting page url\n");
		return;
	}
	
	//Initialize context creation info
	memset(&info, 0, sizeof(info));
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.pprotocols = protocols;
	info.gid = -1;
	info.uid = -1;
	//info.listen_accept_protocol = NULL;
	//info.interface = NULL;
	//info.extensions = lws_get_internal_extensions();
	//info.ssl_cert_filepath = NULL;
	//info.ssl_private_key_filepath = NULL;
	//info.options = NULL;
	
	//Create context
	client = (struct client *)malloc(sizeof(struct client));
	if(client == NULL) {
		printf("Error creating client.\n");
		free(client);
		return;
	}
	//client->context = context; //Maybe causing SIGSEGV?
	context = (lws_ctx_t)malloc(sizeof(lws_ctx_t));
	if(context == NULL) {
		printf("Error creating context.\n");
		free(client);
		return;
	}
	context = lws_create_context(&info);
	client->context = context;
	
	//Initialize client conn info
	memset(&i, 0, sizeof(i));
	i.context = client->context;
	i.port = port;
	i.address = host;
	i.path = path;
	i.host = host;
	i.origin = host;
	i.protocol = protocols[0].name;
	i.pwsi = &client->wsi;

	//Connect to brows
	lws_client_connect_via_info(&i);

	//Run serv loop
	while(1) {
		lws_service(client->context, 6000);
	}
}

static size_t write_response_data(void *contents, size_t size, size_t nmemb, void *userp) {
	size_t realsize = size * nmemb; /*real size of contents*/
	struct response_data *data = (struct response_data *)userp;

	//Allocate mem for the data
	data->data = (char *)realloc(data->data, data->size + realsize + 1);
	if(data->data == NULL) {
		printf("Error allocating memory for data\n");
		free(data->data); /*ensure to free memory reallocation*/
		return 0;
	}

	//Copy contents to data
	memcpy(&(data->data[data->size]), contents, realsize);
	data->size += realsize;
	data->data[data->size] = 0; //null-terminate
	return realsize;
}

//Funct to return pageId from website url
int getPageId(char *url) {
	CURL *curl; /*curl handle*/
	CURLcode res; /*curl res*/
	char *api_url; /*url of wikipedia API*/
	char *query;
	struct response_data data;

	cJSON *json;
	cJSON *query_obj; /*JSON obj for query*/
	cJSON *pages_obj;
	cJSON *page_obj;
	int pageId;

	//Initialize data structure
	data.data = NULL;
	data.size = 0;

	//Set url of wikipedia API
	api_url = "https://en.wikipedia.org/w/api.php";

	//Form the query for wikipedia API
	query = (char *)malloc(strlen(url) + 64);
	sprintf(query, "?action=query&format=json&prop=info&titles=%s&redirects", url);
	/*
	//Shield against SIGSEGV
	CURLcode rs = curl_global_init(CURL_GLOBAL_ALL);
	if(rs != 0) {
		printf("Failed global init...\n");
		return -1;
	}*/
	//Initialize curl handle
	curl = curl_easy_init();
	if(curl) {
		//Set curl opts
		curl_easy_setopt(curl, CURLOPT_URL, api_url);
		curl_easy_setopt(curl, CURLOPT_POSTFIELDS, query);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_response_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, (void *)&data);

		//Perform curl req
		res = curl_easy_perform(curl);
		if(res != CURLE_OK) {
			printf("Error performing curl request: %s\n", curl_easy_strerror(res));
			free(query);
			return -1;
		}

		//Parse resp data as JSON obj
		/*First, ensure it is null-terminated*/
		size_t data_length = strlen(data.data);
		//char *new_data = malloc(data_length + 1);
		if(data.data != NULL) {
			//strcpy(new_data, data.data);
			data.data[data_length] = '\0';
		}
		json = cJSON_Parse(data.data);//TODO: check with new_data instead
		if(json == NULL) {
			printf("Error parsing JSON data: %s\n", cJSON_GetErrorPtr());
			//free(new_data);
			free(query);
			return -1;
		}

		//Get query obj from JSON obj
		query_obj = cJSON_GetObjectItemCaseSensitive(json, "query");
		if(query_obj == NULL) {
			printf("Error getting query obj from JSON obj\n");
			//free(new_data);
			free(query);
			return -1;
		}

		//Get pages obj from pages query obj
		pages_obj = cJSON_GetObjectItemCaseSensitive(query_obj, "pages");
		if(pages_obj == NULL) {
			printf("Error getting pages object form query obj\n");
			//free(new_data);
			free(query);
			return -1;
		}

		//Get first page obj
		page_obj = pages_obj->child;
		if(page_obj == NULL) {
			printf("Error getting page object from pages object\n");
			//free(new_data);
			free(query);
			return -1;
		}

		//Get page ID
		pageId = cJSON_GetObjectItemCaseSensitive(page_obj,"pageid")->valueint;

		//free mem
		cJSON_Delete(json);
		free(data.data);
		//free(new_data);
	} else {
		printf("Curl_easy_init failed..\n");
		return -1;
	}

	//Free mem from query
	free(query);

	//Cleanup curl handle
	curl_easy_cleanup(curl);
	//curl_global_cleanup();

	//Return page ID
	return pageId;
}

//Func to send msg to wsock server
void send_message(struct lws *wsi) {
	char *message; /*Buf for msg*/
	cJSON *json;
	int n;

	//Allocate mem form msg
	message = (char *)malloc(LWS_PRE + 512);

	//Create JSON obj for msg
	json = cJSON_CreateObject();
	if(json == NULL) {
		printf("Error creating JSON object\n");
		return;
	}

	//Add fields to JSON obj
	cJSON_AddNumberToObject(json, "id", 1);
	cJSON_AddStringToObject(json, "method", "Network.enable");

	//Serialize JSON obj to str
	char *json_str = cJSON_PrintUnformatted(json);
	if(json_str == NULL) {
		printf("Error serializing JSON object\n");
		return;
	}

	//Form msg
	sprintf(message + LWS_PRE, "%s", json_str);

	//Write msg
	n = lws_write(wsi, message + LWS_PRE, sizeof(message + LWS_PRE), LWS_WRITE_TEXT);
	if(n < 0) {
		printf("Error writing to websocket connection\n");
		return;
	}

	//Free mems
	free(message);
	cJSON_Delete(json);
	cJSON_free(json_str);
}

void parse_data(char *in) {
	cJSON *json;
	cJSON *id;
	cJSON *method;
	cJSON *params;

	printf("parse_data: in = %s\n", in);
	//Parse data as a JSON obj
	json = cJSON_Parse(in);
	if(json == NULL) {
		printf("Error parsing JSON data: %s\n", cJSON_GetErrorPtr());
		return;
	}

	//Get id obj from JSON
	id = cJSON_GetObjectItemCaseSensitive(json, "id");
	if(id == NULL) {
		printf("Error getting method object from JSON object\n");
		return;
	}

	//Get params obj from JSON obj
	params = cJSON_GetObjectItemCaseSensitive(json, "params");
	if(params == NULL) {
		printf("Error getting params object from JSON object\n");
		return;
	}
	//TODO: implement the func to process data acc to id, method, params
	process_data(id, method, params);

	cJSON_Delete(json);
}

//Func processing data acc to id, method, params
void process_data(cJSON *id, cJSON *method, cJSON *params) {
	//Check if id is valid
	if(id == NULL) {
		printf("Invalid id\n");
		return;
	}

	//Check if method is valid
	if(method == NULL || !cJSON_IsString(method)) {
		printf("Invalid method\n");
		return;
	}

	//Check if params are val
	if(params == NULL || !cJSON_IsObject(params)) {
		printf("Invalid params\n");
		return;
	}
	
	//TODO: implement function to handle diff methods and params
	if(strcmp(method->valuestring, "Network.requestWillBeSent") == 0) {
		//Get req obj from params obj
		cJSON *request = cJSON_GetObjectItemCaseSensitive(params, "request");
		if(request == NULL || !cJSON_GetObjectItemCaseSensitive(params, "request")) {
			printf("Invalid request\n");
			return;
		}

		//Get url str from req obj
		cJSON *url = cJSON_GetObjectItemCaseSensitive(request, "url");
		if(url == NULL || !cJSON_IsString(url)) {
			printf("Invalid url\n");
			return;
		}

		//Print req url
		printf("Request url: %s\n", url->valuestring);
	}
}

void create_and_destroy_context(struct lws_context *context) {
/*	struct lws_context_creation_info info; //Storing context creation info/
	struct lws_context *context;

	//Initialize context creation info
	memset(&info, 0, sizeof(info));
	info.port = CONTEXT_PORT_NO_LISTEN;
	info.pprotocols = NULL;
	info.gid = -1;
	info.uid = -1;
*/
	//Create context
	//context = lws_create_context(&info);
	if(context == NULL) {
		printf("Error creating context\n");
		return;
	}

	pthread_mutex_lock(&mutex);

	lws_context_destroy(context);

	pthread_mutex_unlock(&mutex);

}


