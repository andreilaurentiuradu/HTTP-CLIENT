#ifndef _REQUESTS_
#define _REQUESTS_

// functii pentru tratarea cererilor catre server(GET, POST, DELETE)
char *compute_get_request(char *host, char *url, char *query_params,
                          char **cookies, int cookies_count, char *token);

char *compute_post_request(char *host, char *url, char *content_type,
                           char **body_data, int body_data_fields_count,
                           char **cookies, int cookies_count, char *token);
char *compute_delete_request(char *host, char *url, char *query_params,
                             char **cookies, int cookies_count, char *token);
#endif
