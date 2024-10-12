/* smtp.c */
#include "smtp.h"

void get_input(const char *prompt, char *buf) {
  printf("%s", prompt);
  fgets(buf, MAXINPUT, stdin);
  const int read = strlen(buf);
  if (read > 0) {
    buf[read - 1] = '\0'; // termina nullo
  }
}

/* invia con formattazione del msg */
void send_format(int srv, const char *text, ...) {
  char buf[1024];
  va_list args;
  va_start(args, text); // def dove incomincia la va_list
  vsprintf(buf, text, args);
  va_end(args);

  send(srv, buf, strlen(buf), 0);

  printf("C: %s", buf);
}

/*parsing risposte*/
int parse_response(const char *response) {
  const char *k = response;
  if (!k[0] || !k[1] || !k[2])
    return 0;
  for (; k[3]; ++k) {
    if (k == response || k[-1] == '\n') {
      /*riconosci codici risposta, 3 cifre*/
      if (isdigit(k[0]) && isdigit(k[1]) && isdigit(k[2])) {
        if (k[3] != '-') {
          // ultima riga della risposta
          if (strstr("\r\n", k)) {
            return strtol(k, 0, 10);
          }
        }
      }
    }
  }
}

void wait_on_response(int srv, int expecting) {
  char response[MAXRESPONSE - 1];
  char *p = response; // inizio risp
  char *end = response + MAXRESPONSE;
  int code = 0;

  do {
    int bytes_recvd = recv(srv, p, end - p, 0);
    if (bytes_recvd < 1) {
      fprintf(stderr, "Connessione caduta\n");
      exit(1);
    }
    p += bytes_recvd;
    *p = 0; // valore puntato azzerato

    if (p == end) {
      fprintf(stderr, "Risposta del server troppo grande:\n");
      fprintf(stderr, "%s", response);
      exit(1);
    }
    code = parse_response(response);
  } while (code == 0);
  if (code != expecting) {
    fprintf(stderr, "Errore dal server:\n");
    fprintf(stderr, "%s", response);
    exit(1);
  }
  printf("S: %s", response);
}

// funzione per stabilire la conn
int connect_to_host(const char *hostname, const char *port) {
  struct addrinfo hints;
  memset(&hints, 0, sizeof(hints));
  hints.ai_socktype = SOCK_STREAM; // tcp
  struct addrinfo *peer_addr;      // lista info
  if (getaddrinfo(hostname, port, &hints, &peer_addr) < 0) {
    fprintf(stderr, "errore getaddrinfo: %s", strerror(errno));
    exit(1);
  }
  printf("Indirizzo remoto: ");
  char addr_buf[100];
  char serv_buf[100];
  // ottieni addr_buf e serv_buf, remoti
  getnameinfo(peer_addr->ai_addr, peer_addr->ai_addrlen, addr_buf,
              sizeof(addr_buf), serv_buf, sizeof(serv_buf),
              NI_NUMERICHOST); // in forma numerica
  printf("%s %s\n", addr_buf, serv_buf);

  int serv_sock;
  serv_sock = socket(peer_addr->ai_family, peer_addr->ai_socktype,
                     peer_addr->ai_protocol);
  if (serv_sock < 0) {
    fprintf(stderr, "errore socket: %s\n", strerror(errno));
    exit(1);
  }
  printf("Connessione...\n");
  if (connect(serv_sock, peer_addr->ai_addr, peer_addr->ai_addrlen) < 0) {
    fprintf(stderr, "errore connect: %s\n", strerror(errno));
    exit(1);
  }
  freeaddrinfo(peer_addr); // libera
  return serv_sock;
}

int main() {
  char hostname[MAXINPUT];
  get_input("mail server: ", hostname);
  printf("connessione a host: %s:25\n", hostname);
  int server = connect_to_host(hostname, "25");
  //server conferma inizio operazioni
  wait_on_response(server, 220);
  send_format(server, "HELO HONPWC\r\n");
  wait_on_response(server, 250); //250: codice ok

  char sender[MAXINPUT];
  get_input("from: ",  sender);
  send_format(server, "MAIL FROM:<%s>\r\n", sender);
  wait_on_response(server, 250);
  
  char recipient[MAXINPUT];
  get_input("to: ", recipient);
  send_format(server, "RCPT TO: <%s>\r\n", recipient);
  wait_on_response(server, 250);

  send_format(server, "DATA\r\n");
  wait_on_response(server, 354);

  char subject[MAXINPUT];
  get_input("subject: ", subject);
  send_format(server, "From:<%s>\r\n", sender);
  send_format(server, "To:<%s>\r\n", recipient);
  send_format(server, "Subject:%s\r\n", subject);

  time_t timer;
  time(&timer);

  struct tm *timeinfo;
  timeinfo = gmtime(&timer);

  char date[128];
  strftime(date, 128, "%a, %d %b %Y %H:%M:%S +0000", timeinfo);
  send_format(server, "Date:%s\r\n",date);

  send_format(server, "\r\n");
  //immetti testo msg
  printf("Immetti il testo del msg, termina con \".\" in una nuova linea:\n");

  while(1) {
    char body[MAXINPUT];
    get_input("> ", body);
    send_format(server, "%s\r\n", body);
    if(strcmp(body, ".") == 0) {
      break;
    }
  }
  wait_on_response(server, 250);
  send_format(server, "QUIT\r\n");
  wait_on_response(server, 221);

  //chiudi socket
  close(server);
  return 0;
}
