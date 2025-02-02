#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
    char *data;
    size_t size;
} buffer;

// functii pentru manipularea unei structuri de tip buffer
buffer buffer_init(void);

void buffer_destroy(buffer *buffer);

void buffer_add(buffer *buffer, const char *data, size_t data_size);

int buffer_is_empty(buffer *buffer);

int buffer_find(buffer *buffer, const char *data, size_t data_size);

int buffer_find_insensitive(buffer *buffer, const char *data, size_t data_size);