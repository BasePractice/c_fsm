// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include <memory.h>
#include "common.h"

#define ALLOCATED_BUFFER_LEN 256
#define ENDL '\n'

static void
string_read_fd_end(FILE *fd, struct String *string, int stop_symbol) {
    if (0 != fd && 0 != string) {
        while (!feof(fd)) {
            int ch = fgetc(fd);
            if (ch == stop_symbol || EOF == ch)
                break;
            string_add_symbol(string, (char) ch);
        }
    }
}

void string_add_symbol(struct String *string, char ch) {
    if (0 != string) {
        if (0 == string->text) {
            string->text = calloc(ALLOCATED_BUFFER_LEN, 1);
            string->text_allocated = ALLOCATED_BUFFER_LEN;
            string->text_len = 0;
        } else if (string->text_len >= string->text_allocated) {
            string->text_allocated += ALLOCATED_BUFFER_LEN;
            string->text = realloc(string->text, string->text_allocated);
        }
        string->text[string->text_len++] = ch;
    }
}

void string_read_fd(FILE *fd, struct String *string) {
    string_read_fd_end(fd, string, EOF);
}

void string_read_line(FILE *fd, struct String *string) {
    string_read_fd_end(fd, string, ENDL);
}

void string_reset(struct String *string) {
    if (0 != string) {
        memset(string->text, 0, string->text_allocated);
        string->text_len = 0;
    }
}

void string_destroy(struct String *string) {
    if (0 != string) {
        if (0 != string->text) {
            free(string->text);
        }
        string->text = 0;
        string->text_len = 0;
        string->text_allocated = 0;
    }
}

static const char hex[] = "0123456789ABCDEF";

bool
binary_to_hex(const uint8_t *binary, size_t binary_size, char *hex_text, size_t *hex_text_size) {
    int i;

    if (binary == 0 || hex_text == 0 || binary_size == 0 || hex_text_size == 0)
        return false;
    if (binary_size * 2 > (*hex_text_size))
        return false;
    memset(hex_text, 0, (*hex_text_size));
    for (i = 0; i < binary_size; ++i) {
        int k = i * 2;
        hex_text[k] = hex[(binary[i] >> 4) & 0xF];
        hex_text[k + 1] = hex[(binary[i]) & 0xF];
    }
    return true;
}

size_t base64_encode(const uint8_t *in, size_t len, uint8_t *out, size_t out_len, int line_length);

int
b64_binary_to_base64(const uint8_t *binary, size_t binary_size, char *base64_text, size_t *base64_text_size) {
    if (binary == NULL || binary_size == 0 || base64_text == NULL || base64_text_size == NULL ||
        (*base64_text_size) == 0)
        return 0;
    (*base64_text_size) = base64_encode(binary, binary_size, base64_text, (*base64_text_size), -1);
    return (*base64_text_size) > 0;
}

void matrix_allocate(struct MapMatrix *matrix, size_t size) {
    if (0 != matrix) {
        size_t i;
        matrix->data = (MapMatrixValue **) calloc(size, sizeof(MapMatrixValue **));
        for (i = 0; i < size; ++i) {
            matrix->data[i] = (MapMatrixValue *) calloc(size, sizeof(MapMatrixValue *));
        }
        matrix->size = size;
    }
}

void matrix_put(struct MapMatrix *matrix, size_t row, size_t col, MapMatrixValue value) {
    assert(0 != matrix && 0 != matrix->data && 0 != matrix->size && row >= 0
           && row < matrix->size && col >= 0 && col < matrix->size);
    matrix->data[row][col] = value;
}

MapMatrixValue matrix_get(const struct MapMatrix *matrix, size_t row, size_t col) {
    assert(0 != matrix && 0 != matrix->data && 0 != matrix->size && row >= 0
           && row < matrix->size && col >= 0 && col < matrix->size);
    return matrix->data[row][col];
}

void matrix_destroy(struct MapMatrix *matrix) {
    if (0 != matrix && 0 != matrix->data && 0 != matrix->size) {
        size_t i;

        for (i = 0; i < matrix->size; ++i) {
            free(matrix->data[i]);
        }
        free(matrix->data);
        matrix->data = 0;
        matrix->size = 0;
    }
}

