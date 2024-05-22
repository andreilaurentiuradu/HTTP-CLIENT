#include "buffer.h"

// initializarea unui buffer
buffer buffer_init(void) {
    buffer buffer;

    buffer.data = NULL;
    buffer.size = 0;

    return buffer;
}

// distrugerea bufferului si eliberarea memoriei
void buffer_destroy(buffer *buffer) {
    if (buffer->data != NULL) {
        free(buffer->data);
        buffer->data = NULL;
    }

    buffer->size = 0;
}

// verificarea daca este gol
int buffer_is_empty(buffer *buffer) { return buffer->data == NULL; }

// adaugare in buffer
void buffer_add(buffer *buffer, const char *data, size_t data_size) {
    if (buffer->data != NULL) {
        buffer->data =
            realloc(buffer->data, (buffer->size + data_size) * sizeof(char));
    } else {
        buffer->data = calloc(data_size, sizeof(char));
    }

    memcpy(buffer->data + buffer->size, data, data_size);

    buffer->size += data_size;
}

// cautarea unui buffer prin parcurgerea sa liniara
int buffer_find(buffer *buffer, const char *data, size_t data_size) {
    if (data_size > buffer->size) return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (buffer->data[i + j] != data[j]) {
                break;
            }
        }

        if (j == data_size) return i;
    }

    return -1;
}

// buffer_find varianta case insensitive
int buffer_find_insensitive(buffer *buffer, const char *data,
                            size_t data_size) {
    if (data_size > buffer->size) return -1;

    size_t last_pos = buffer->size - data_size + 1;

    for (size_t i = 0; i < last_pos; ++i) {
        size_t j;

        for (j = 0; j < data_size; ++j) {
            if (tolower(buffer->data[i + j]) != tolower(data[j])) {
                break;
            }
        }

        if (j == data_size) return i;
    }

    return -1;
}
