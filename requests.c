#include "requests.h"

#include <arpa/inet.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"

char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // scriem tipul cererii, URL-ul, parametrii cererii (daca sunt) si tipul
    // protocolului
    if (query_params != NULL) {
        sprintf(line, "GET %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "GET %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // adaugam hostul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaugam headere si cookies
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // adaugam tokenul
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }
    // enter la final
    compute_message(message, "");
    return message;
}

char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    // scriem tipul cererii, URL-ul, parametrii cererii (daca sunt) si tipul
    // protocolului
    if (query_params != NULL) {
        sprintf(line, "DELETE %s?%s HTTP/1.1", url, query_params);
    } else {
        sprintf(line, "DELETE %s HTTP/1.1", url);
    }

    compute_message(message, line);

    // adaugam hostul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaugam headere si cookies
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            sprintf(line, "Cookie: %s", cookies[i]);
            compute_message(message, line);
        }
    }

    // adaugam tokenul
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // adaugam enter
    compute_message(message, "");
    return message;
}

char *compute_post_request(char *host, char *url, char *content_type,
                           char **body_data, int body_data_fields_count,
                           char **cookies, int cookies_count, char *token) {
    char *message = calloc(BUFLEN, sizeof(char));
    char *line = calloc(LINELEN, sizeof(char));
    char *body_data_buffer = calloc(LINELEN, sizeof(char));

    // scriem numele metodei, URL-ul si tipul protocolului
    sprintf(line, "POST %s HTTP/1.1", url);
    compute_message(message, line);

    // adaugam hostul
    sprintf(line, "Host: %s", host);
    compute_message(message, line);

    // adaugam date despre content
    sprintf(line, "Content-Type: %s", content_type);
    compute_message(message, line);
    int content_length = 0;
    for (int i = 0; i < body_data_fields_count; i++) {
        content_length += strlen(body_data[i]);
    }
    if (body_data_fields_count > 1) {
        content_length += body_data_fields_count - 1;
    }

    sprintf(line, "Content-Length: %d", content_length);
    compute_message(message, line);

    // adaugam cookies
    if (cookies != NULL) {
        for (int i = 0; i < cookies_count; i++) {
            compute_message(message, cookies[i]);
        }
    }

    // adaugam tokenul
    if (token != NULL) {
        sprintf(line, "Authorization: Bearer %s", token);
        compute_message(message, line);
    }

    // adaugam enter
    compute_message(message, "");

    // adaugam payloadul
    for (int i = 0; i < body_data_fields_count; i++) {
        strcat(body_data_buffer, body_data[i]);
        if (i != body_data_fields_count - 1) {
            strcat(body_data_buffer, "&");
        }
    }
    memset(line, 0, LINELEN);
    strcat(message, body_data_buffer);

    // eliberam memoria
    free(line);
    free(body_data_buffer);

    // intoarcem mesajul
    return message;
}
