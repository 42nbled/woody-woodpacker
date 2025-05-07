#include "woody.h"

void insert_str(char **buffer, const char *str, int index, size_t *size) {
    size_t str_len = strlen(str);
    size_t new_size = *size + str_len;

    char *new_buffer = malloc(new_size);
    if (!new_buffer) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    memcpy(new_buffer, *buffer, index);                      // Copier début ELF
    memcpy(new_buffer + index, str, str_len);               // Insérer payload
    memcpy(new_buffer + index + str_len, *buffer + index, *size - index); // Copier reste

    free(*buffer);
    *buffer = new_buffer;
    *size = new_size;  // Mettre à jour la taille
}


