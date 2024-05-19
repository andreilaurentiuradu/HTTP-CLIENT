#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdio.h>      /* printf, sprintf */
#include <stdlib.h>     /* exit, atoi, malloc, free */
#include <string.h>     /* memcpy, memset */
#include <sys/socket.h> /* socket, connect */
#include <unistd.h>     /* read, write, close */

#include "helpers.h"
#include "parson.h"
#include "requests.h"

int main(int argc, char *argv[]) {
    char *msg;
    char *resp;
    int sockfd;
    char *token = malloc(300);
    char *cookie = malloc(200);
    char *code = malloc(3);

    while (1) {
        char task[14], username[100], password[100];
        scanf("%s", task);

        if (strncmp(task, "register", strlen("register")) == 0) {
            JSON_Object *obj;
            JSON_Value *root_value = json_value_init_object();
            obj = json_value_get_object(root_value);
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            // cream obiectul json
            json_object_set_string(obj, "username", username);
            json_object_set_string(obj, "password", password);

            // Print JSON object
            char *serialized_string =
                json_serialize_to_string_pretty(root_value);
            printf("JSON Output:\n%s\n", serialized_string);

            // Ex 1.1: GET dummy from main server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
            msg = compute_post_request(
                "34.246.184.49", "/api/v1/tema/auth/register",
                "application/json", &serialized_string, 1, NULL, 0, token);
            send_to_server(sockfd, msg);
            resp = receive_from_server(sockfd);
            // printf("%s\n", resp);

            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Utilizator înregistrat cu succes!\n");
            } else {
                printf("Eroare: User deja înregistrat.\n");
            }
        } else if (strncmp("login", task, strlen("login")) == 0) {
            JSON_Object *obj2;
            JSON_Value *root_value2 = json_value_init_object();
            obj2 = json_value_get_object(root_value2);
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            // cream obiectul json
            json_object_set_string(obj2, "username", username);
            json_object_set_string(obj2, "password", password);

            // Print JSON object
            char *serialized_string2 =
                json_serialize_to_string_pretty(root_value2);
            printf("JSON Output:\n%s\n", serialized_string2);

            // Ex 1.1: GET dummy from main server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
            msg = compute_post_request(
                "34.246.184.49", "/api/v1/tema/auth/login", "application/json",
                &serialized_string2, 1, NULL, 0, token);
            send_to_server(sockfd, msg);
            resp = receive_from_server(sockfd);
            // printf("%s\n", resp);

            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                char *cookies = strstr(resp, "Set-Cookie: ");
                strcpy(cookie, strtok(cookies + strlen("Set-Cookie: "), ";"));
                printf("Utilizator logat cu succes!\n");
                // printf("Cookieul este %s.\n", cookie);
            } else {
                printf("Eroare: Userul nu se poate loga.\n");
            }
        } else if (strncmp("enter_library", task, strlen("enter_library")) ==
                   0) {
            // Ex 1.1: GET dummy from main server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
            msg = compute_get_request("34.246.184.49",
                                      "/api/v1/tema/library/access", NULL,
                                      &cookie, 1, token);
            send_to_server(sockfd, msg);
            resp = receive_from_server(sockfd);
            // printf("%s\n", resp);

            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");
            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Utilizatorul are acces la biblioteca\n");
                token =
                    strstr(basic_extract_json_response(resp), "\"token\":\"") +
                    strlen("\"token\":\"");
                token = strtok(token, "\"");
                // printf("token: %s\n", token);
            } else {
                printf("Eroare: Acces respins.\n");
            }
        } else if (strncmp("get_books", task, strlen("get_books")) == 0) {
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
            msg = compute_get_request("34.246.184.49",
                                      "/api/v1/tema/library/books", NULL,
                                      &cookie, 1, token);
            send_to_server(sockfd, msg);
            resp = receive_from_server(sockfd);
            // printf("%s\n", resp);

            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");
            // succes
            if (strncmp(code, "20", 2) == 0) {
                char *books = strstr(resp, "[{\"");
                if (books != NULL) {
                    printf("%s\n", books);
                }
            } else {
                printf("Eroare: Acces respins.\n");
            }
        } else if (strncmp("get_book", task, strlen("get_book")) == 0) {
            char *id = malloc(10);
            printf("id=");
            scanf("%s", id);
            // TODO verifici daca id-ul e string si afisezi eroare
            int id_valid = 1;
            for (int i = 0; i < strlen(id); ++i) {
                if ('0' > id[i] || '9' < id[i]) {
                    id_valid = 0;
                }
            }
            if (!id_valid) {
                printf("Eroare: Id-ul contine caractere interzise.\n");
                continue;
            }

            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);
            char URL[100] = "/api/v1/tema/library/books/";
            strcat(URL, id);
            msg = compute_get_request("34.246.184.49", URL, NULL, &cookie, 1,
                                      token);
            send_to_server(sockfd, msg);
            resp = receive_from_server(sockfd);
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("%s\n", basic_extract_json_response(resp));
            } else {
                printf("Eroare: Cartea nu exista.\n");
            }

        } else if (strncmp("add_book", task, strlen("add_book")) == 0) {
            char title[100];
            char author[100];
            char genre[100];
            char publisher[100];
            char page_count[100];

            printf("title=");
            fgets("%s", title, stdin);
            printf("author=");
            fgets("%s", author, stdin);
            printf("genre=");
            fgets("%s", genre, stdin);
            printf("publisher=");
            fgets("%s", publisher, stdin);
            printf("page_count=");
            scanf("%s", page_count);

            int page_count_valid = 1;
            for (int i = 0; i < strlen(page_count); ++i) {
                if ('0' > page_count[i] || '9' < page_count[i]) {
                    page_count_valid = 0;
                }
            }
            if (!page_count_valid) {
                printf(
                    "Eroare:  Tip de date incorect pentru numarul de "
                    "pagini.\n");
                continue;
            }

            JSON_Object *obj3;
            JSON_Value *root_value3 = json_value_init_object();
            obj3 = json_value_get_object(root_value3);
            // cream obiectul json
            json_object_set_string(obj3, "title", title);
            json_object_set_string(obj3, "author", author);
            json_object_set_string(obj3, "genre", genre);
            json_object_set_string(obj3, "publisher", publisher);
            json_object_set_number(obj3, "page_count", (int)page_count);

            // Print JSON object
            char *serialized_string3 =
                json_serialize_to_string_pretty(root_value3);
            printf("JSON Output:\n%s\n", serialized_string3);
        }
        // // Ex 1.2: POST dummy and print response from main server
        // char **data = calloc(1, sizeof(char *));
        // data[0] = calloc(100, sizeof(char));
        // strcpy(data[0], "test&dummy");
        // msg = compute_post_request("34.246.184.49", "/api/v1/dummy",
        //                            "application/x-www-form-urlencoded",
        //                            data, 1, NULL, 0);
        // send_to_server(sockfd, msg);
        // resp = receive_from_server(sockfd);
        // printf("%s\n", resp);

        // // Ex 2: Login into main server
        // char **data2 = calloc(2, sizeof(char *));
        // data2[0] = calloc(100, sizeof(char));
        // data2[1] = calloc(100, sizeof(char));
        // strcpy(data2[0], "username=student");
        // strcpy(data2[1], "password=student");
        // msg = compute_post_request("34.246.184.49", "/api/v1/auth/login",
        //                            "application/x-www-form-urlencoded",
        //                            data2, 2, NULL, 0);
        // send_to_server(sockfd, msg);
        // resp = receive_from_server(sockfd);
        // printf("%s\n", resp);

        // // Ex 3: GET weather key from main server
        // int sockfd2 =
        //     open_connection("82.196.7.246", 80, AF_INET, SOCK_STREAM, 0);
        // char **cookies = calloc(1, sizeof(char *));
        // cookies[0] = calloc(1000, sizeof(char));
        // strcpy(cookies[0],
        //        "connect.sid=s%3A8c0LVV6Nbuwi0cX0fhcw0xo7EAoeQr6L.TT%"
        //        "2Fp1fPyCi6q6W09wayoKjrd%2FrYyTm3Gf299QRkmM9Y");
        // msg = compute_get_request("82.196.7.246", "/api/v1/weather/key",
        // NULL,
        //                           cookies, 1);
        // send_to_server(sockfd2, msg);
        // resp = receive_from_server(sockfd2);
        // printf("%s\n", resp);
    }

    return 0;
}
