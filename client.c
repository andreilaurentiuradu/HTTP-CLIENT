#include <arpa/inet.h>
#include <ctype.h>
#include <netdb.h>      /* struct hostent, gethostbyname */
#include <netinet/in.h> /* struct sockaddr_in, struct sockaddr */
#include <stdbool.h>
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
    char *token = malloc(3000);
    char *cookie = malloc(2000);
    char *code = malloc(3);
    bool entered_in_library = false;

    while (1) {
        char task[14], username[100], password[100];

        // citim comanda
        scanf("%s", task);

        // daca userul vrea sa se inregistreze
        if (strncmp(task, "register", strlen("register")) == 0) {
            // ii cerem un nume si o parola
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            // cream obiectul json
            JSON_Object *obj;
            JSON_Value *root_value = json_value_init_object();
            obj = json_value_get_object(root_value);
            json_object_set_string(obj, "username", username);
            json_object_set_string(obj, "password", password);

            // il convertim intr-un string
            char *serialized_string =
                json_serialize_to_string_pretty(root_value);

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // pastram informatia primita de la api prin cererea de tip POST
            msg = compute_post_request(
                "34.246.184.49", "/api/v1/tema/auth/register",
                "application/json", &serialized_string, 1, NULL, 0, token);

            // trimitem serverului
            send_to_server(sockfd, msg);

            // stocam raspunsul de la server
            resp = receive_from_server(sockfd);

            // stocam codul intors
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Utilizator inregistrat cu succes!\n");
            } else {
                printf("Eroare: User deja inregistrat.\n");
            }
        } else if (strncmp("login", task, strlen("login")) == 0) {
            // daca userul vrea sa se logheze
            // ii cerem datele de conectare
            printf("username=");
            scanf("%s", username);
            printf("password=");
            scanf("%s", password);

            // cream obiectul json cu datele userului
            JSON_Object *obj2;
            JSON_Value *root_value2 = json_value_init_object();
            obj2 = json_value_get_object(root_value2);
            json_object_set_string(obj2, "username", username);
            json_object_set_string(obj2, "password", password);

            // il convertim intr-un string
            char *serialized_string2 =
                json_serialize_to_string_pretty(root_value2);

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // pastram informatia primita de la api prin cererea de tip POST
            msg = compute_post_request(
                "34.246.184.49", "/api/v1/tema/auth/login", "application/json",
                &serialized_string2, 1, NULL, 0, token);
            // trimitem serverului
            send_to_server(sockfd, msg);

            // stocam raspunsul de la server
            resp = receive_from_server(sockfd);

            // stocam codul intors
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("doi\n");

                // pastram din raspunsul serverului incepand cu primul cookie
                char *cookies = strstr(resp, "Set-Cookie: ");
                printf("cookies:%s\n", cookies);
                strcpy(cookie, strtok(cookies + strlen("Set-Cookie: "), ";"));
                printf("cookie:%s", cookie);
                printf("Utilizator logat cu succes!\n");
            } else {
                printf("Eroare: Userul nu se poate loga.\n");
            }
        } else if (strncmp("enter_library", task, strlen("enter_library")) ==
                   0) {
            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // stocam mesajul intors de api dupa cererea de tip GET
            msg = compute_get_request("34.246.184.49",
                                      "/api/v1/tema/library/access", NULL,
                                      &cookie, 1, token);

            // trimitem rezultatul catre server
            send_to_server(sockfd, msg);

            // stocam raspunsul primit de la server
            resp = receive_from_server(sockfd);

            // stocam codul intors
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Utilizatorul are acces la biblioteca.\n");
                entered_in_library = true;

                // stocam incepand cu aparitia tokenului
                token =
                    strstr(basic_extract_json_response(resp), "\"token\":\"") +
                    strlen("\"token\":\"");

                // pastram doar tokenul
                token = strtok(token, "\"");
            } else {
                printf("Eroare: Acces respins.\n");
            }
        } else if (strncmp("get_books", task, strlen("get_books")) == 0) {
            // daca utilizatorul vrea sa vada cartile din biblioteca

            if (!entered_in_library) {
                // daca nu se afla in biblioteca
                printf("Utilizatorul nu se afla in biblioteca!\n");
                continue;
            }

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // stocam mesajul primit de la api prin cererea de tip GET
            msg = compute_get_request("34.246.184.49",
                                      "/api/v1/tema/library/books", NULL,
                                      &cookie, 1, token);

            // trimitem raspunsul primit catre server
            send_to_server(sockfd, msg);

            // preluam raspunsul de la server
            resp = receive_from_server(sockfd);

            // stocam codul primit de la server
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");
            // succes
            if (strncmp(code, "20", 2) == 0) {
                // afisam cartile
                char *books = strstr(resp, "[{\"");
                if (books != NULL) {
                    printf("%s\n", books);
                }
            } else {
                printf("Eroare: Acces respins.\n");
            }
        } else if (strncmp("get_book", task, strlen("get_book")) == 0) {
            // daca utilizatorul doreste datele unei carti cu un id specific
            if (!entered_in_library) {
                printf("Utilizatorul nu se afla in biblioteca!\n");
                continue;
            }

            // citim id-ul cartii ca string
            char *id = malloc(10);
            printf("id=");
            scanf("%s", id);
            // verificam daca id-ul este format doar din cifre
            int id_valid = 1;
            for (int i = 0; i < strlen(id); ++i) {
                if ('0' > id[i] || '9' < id[i]) {
                    id_valid = 0;
                }
            }

            // daca id-ul contine caracter nonnumerice
            if (!id_valid) {
                printf("Eroare: Id-ul contine caractere interzise.\n");
                continue;
            }

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // stocam url-ul fara id
            char URL[100] = "/api/v1/tema/library/books/";

            // concatenam id-ul
            strcat(URL, id);

            // salvam raspunsul primit de la api dupa cererea de tip GET
            msg = compute_get_request("34.246.184.49", URL, NULL, &cookie, 1,
                                      token);

            // trimitem serverului
            send_to_server(sockfd, msg);

            // raspunsul primit de la server
            resp = receive_from_server(sockfd);

            // codul preluat din raspunsul de la server
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("%s\n", basic_extract_json_response(resp));
            } else {
                printf("Eroare: Cartea nu exista.\n");
            }

        } else if (strncmp("add_book", task, strlen("add_book")) == 0) {
            // daca utilizatorul doreste sa adauge o carte in biblioteca
            if (!entered_in_library) {
                // daca nu se afla in biblioteca
                printf("Utilizatorul nu se afla in biblioteca!\n");
                continue;
            }
            char title[100];
            char author[100];
            char genre[100];
            char publisher[100];
            char page_count[100];

            char enter[2];
            fgets(enter, 2, stdin);

            // citim datele cartii ce va fi adaugata
            printf("title=");
            fgets(title, 100, stdin);
            title[strlen(title) - 1] = '\0';

            printf("author=");
            fgets(author, 100, stdin);
            author[strlen(author) - 1] = '\0';

            printf("genre=");
            fgets(genre, 100, stdin);
            genre[strlen(genre) - 1] = '\0';

            printf("publisher=");
            fgets(publisher, 100, stdin);
            publisher[strlen(publisher) - 1] = '\0';

            printf("page_count=");
            scanf("%s", page_count);

            // verificam ca numarul de pagini contine doar numere
            int page_count_valid = 1;
            for (int i = 0; i < strlen(page_count); ++i) {
                if ('0' > page_count[i] || '9' < page_count[i]) {
                    page_count_valid = 0;
                }
            }
            // nu contine alte caractere si nu incepe cu 0
            if (!page_count_valid) {
                printf(
                    "Eroare:  Tip de date incorect pentru numarul de "
                    "pagini.\n");
                continue;
            }

            // cream obiectul json
            JSON_Object *obj3;
            JSON_Value *root_value3 = json_value_init_object();
            obj3 = json_value_get_object(root_value3);

            json_object_set_string(obj3, "title", title);
            json_object_set_string(obj3, "author", author);
            json_object_set_string(obj3, "genre", genre);
            json_object_set_string(obj3, "publisher", publisher);
            json_object_set_number(obj3, "page_count", atoi(page_count));

            // facem conversia la string
            char *serialized_string3 =
                json_serialize_to_string_pretty(root_value3);

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // stocam raspunsul primit de api prin cererea de tip request
            msg = compute_post_request(
                "34.246.184.49", "/api/v1/tema/library/books",
                "application/json", &serialized_string3, 1, NULL, 0, token);

            // trimitem catre server
            send_to_server(sockfd, msg);

            // stocam rezultatul primit de la server
            resp = receive_from_server(sockfd);

            // preluam codul din raspunsul de la server
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Carte adaugata cu succes!\n");
            } else {
                printf("Eroare:\n");
            }
        } else if (strncmp("delete_book", task, strlen("delete_book")) == 0) {
            char *id2 = malloc(10);
            printf("id=");
            scanf("%s", id2);

            // verificam daca id-ul contine doar cifre
            int id_valid2 = 1;
            for (int i = 0; i < strlen(id2); ++i) {
                if ('0' > id2[i] || '9' < id2[i]) {
                    id_valid2 = 0;
                }
            }
            if (!id_valid2) {
                printf("Eroare: Id-ul contine caractere interzise.\n");
                continue;
            }

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // cream url-ul
            char URL2[100] = "/api/v1/tema/library/books/";
            strcat(URL2, id2);

            // stocam raspunsul primit de la api dupa o cerere de tip DELETE
            msg = compute_delete_request("34.246.184.49", URL2, NULL, &cookie,
                                         1, token);

            // trimitem catre server
            send_to_server(sockfd, msg);

            // stocam raspunsul de la server
            resp = receive_from_server(sockfd);

            // preluam din raspuns codul intors
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                printf("Cartea cu id-ul %s a fost stearsa cu succes.\n", id2);
            } else {
                printf("Eroare: Cartea cu id-ul %s nu exista.\n", id2);
            }
        } else if (strncmp("logout", task, strlen("logout")) == 0) {
            // daca utilizatorul vrea sa se delogheze
            entered_in_library = false;

            // deschidem conexiunea catre server
            sockfd =
                open_connection("34.246.184.49", 8080, AF_INET, SOCK_STREAM, 0);

            // preluam mesajul intors de api in urma cererii de tip GET
            msg =
                compute_get_request("34.246.184.49", "/api/v1/tema/auth/logout",
                                    NULL, &cookie, 1, token);

            // trimitem catre server
            send_to_server(sockfd, msg);

            // stocam raspunsul de la server
            resp = receive_from_server(sockfd);

            // preluam din raspuns codul intors
            code = strstr(resp, "HTTP/1.1 ") + strlen("HTTP/1.1 ");

            // succes
            if (strncmp(code, "20", 2) == 0) {
                token = NULL;
                cookie = NULL;
                printf("Utilizatorul s-a delogat cu succes!\n");
            } else {
                printf("Eroare: Delogare nereusita.\n");
            }
        } else if (strncmp("exit", task, strlen("exit")) == 0) {
            // daca utilizatorul vrea sa inchida aplicatia
            // inchidem conexiunea catre server
            close(sockfd);
            break;
        } else {
            printf("Comanda invalida! Incearca din nou!\n");
        }
    }

    return 0;
}
