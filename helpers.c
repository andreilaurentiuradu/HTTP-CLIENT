#include "helpers.h"  // Functii helper personalizate, presupuse a fi furnizate

#include <arpa/inet.h>   // inet_aton
#include <netdb.h>       // struct hostent, gethostbyname
#include <netinet/in.h>  // struct sockaddr_in, struct sockaddr, htons
#include <stdio.h>       // printf, sprintf
#include <stdlib.h>      // exit, atoi, malloc, free
#include <string.h>      // memcpy, memset, strcat, strlen
#include <sys/socket.h>  // socket, connect
#include <unistd.h>      // read, write, close

#include "buffer.h"  // Functii buffer personalizate, presupuse a fi furnizate

// Constante pentru headerele HTTP
#define HEADER_TERMINATOR "\r\n\r\n"
#define HEADER_TERMINATOR_SIZE (sizeof(HEADER_TERMINATOR) - 1)
#define CONTENT_LENGTH "Content-Length: "
#define CONTENT_LENGTH_SIZE (sizeof(CONTENT_LENGTH) - 1)

// Functie pentru a afisa un mesaj de eroare si a iesi
void error(const char *msg) {
    perror(msg);
    exit(0);
}

// Functie pentru a adauga o linie la un mesaj HTTP
void compute_message(char *message, const char *line) {
    strcat(message, line);
    strcat(message, "\r\n");
}

// Functie pentru a deschide o conexiune socket catre un server
int open_connection(char *host_ip, int portno, int ip_type, int socket_type,
                    int flag) {
    struct sockaddr_in serv_addr;
    int sockfd = socket(ip_type, socket_type, flag);  // Creaza socket-ul
    if (sockfd < 0) error("ERROR opening socket");

    memset(
        &serv_addr, 0,
        sizeof(
            serv_addr));  // Initializeaza structura adresei serverului cu zero
    serv_addr.sin_family = ip_type;  // Seteaza familia de adrese (IPv4/IPv6)
    serv_addr.sin_port = htons(portno);  // Seteaza numarul portului
    inet_aton(
        host_ip,
        &serv_addr.sin_addr);  // Converteste adresa IP din text in forma binara

    // Conecteaza socket-ul la server
    if (connect(sockfd, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)
        error("ERROR connecting");

    return sockfd;  // Returneaza descriptorul de fisier al socket-ului
}

// Functie pentru a inchide o conexiune socket
void close_connection(int sockfd) { close(sockfd); }

// Functie pentru a trimite un mesaj catre server
void send_to_server(int sockfd, char *message) {
    int bytes, sent = 0;
    int total = strlen(message);  // Obtine lungimea totala a mesajului

    do {
        bytes = write(sockfd, message + sent,
                      total - sent);  // Scrie mesajul in socket
        if (bytes < 0) {
            error("ERROR writing message to socket");
        }

        if (bytes == 0) {
            break;
        }

        sent += bytes;  // Actualizeaza numarul de bytes trimisi
    } while (sent < total);  // Continua pana cand intregul mesaj este trimis
}

// Functie pentru a primi un raspuns de la server
char *receive_from_server(int sockfd) {
    char response[BUFLEN];          // Buffer pentru a stoca raspunsul
    buffer buffer = buffer_init();  // Initializeaza un buffer personalizat
    int header_end = 0;
    int content_length = 0;

    // Citeste raspunsul de la server
    do {
        int bytes = read(sockfd, response, BUFLEN);  // Citeste bytes din socket

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response,
                   (size_t)bytes);  // Adauga bytes in buffer-ul personalizat

        // Cauta terminatorul header-ului
        header_end =
            buffer_find(&buffer, HEADER_TERMINATOR, HEADER_TERMINATOR_SIZE);

        if (header_end >= 0) {
            header_end += HEADER_TERMINATOR_SIZE;

            // Cauta lungimea continutului
            int content_length_start = buffer_find_insensitive(
                &buffer, CONTENT_LENGTH, CONTENT_LENGTH_SIZE);

            if (content_length_start < 0) {
                continue;
            }

            content_length_start += CONTENT_LENGTH_SIZE;
            content_length =
                strtol(buffer.data + content_length_start, NULL, 10);
            break;
        }
    } while (1);

    size_t total = content_length + (size_t)header_end;

    // Continua citirea pana cand intregul continut este primit
    while (buffer.size < total) {
        int bytes = read(sockfd, response, BUFLEN);

        if (bytes < 0) {
            error("ERROR reading response from socket");
        }

        if (bytes == 0) {
            break;
        }

        buffer_add(&buffer, response, (size_t)bytes);
    }

    buffer_add(&buffer, "", 1);  // Adauga terminatorul de string
    return buffer.data;          // Returneaza datele din buffer
}

// Functie pentru a extrage un raspuns JSON de baza dintr-un string
char *basic_extract_json_response(char *str) { return strstr(str, "{\""); }