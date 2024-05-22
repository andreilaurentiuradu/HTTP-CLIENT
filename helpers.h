#ifndef _HELPERS_
#define _HELPERS_

#define BUFLEN 4096
#define LINELEN 1000

// Afiseaza eroarea curenta
void error(const char *msg);

// Adauga o linie la un mesaj de tip string
void compute_message(char *message, const char *line);

// Deschide o conexiune cu serverul host_ip pe portul portno, returneaza un
// socket
int open_connection(char *host_ip, int portno, int ip_type, int socket_type,
                    int flag);

// Inchide o conexiune server pe socket-ul sockfd
void close_connection(int sockfd);

// Trimite un mesaj catre un server
void send_to_server(int sockfd, char *message);

// Primeste si returneaza mesajul de la un server
char *receive_from_server(int sockfd);

// Extrage si returneaza un JSON din raspunsul serverului
char *basic_extract_json_response(char *str);

#endif