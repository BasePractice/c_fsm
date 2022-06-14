// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_COMMON_H
#define C_FSM_COMMON_H

#include <stdio.h>
#include <stdbool.h>
#include <stdint.h>

#if defined(__cplusplus)
extern "C" {
#endif

typedef void * MapMatrixValue;
struct MapMatrix {
    MapMatrixValue **data;
    size_t        size;
};

void matrix_allocate(struct MapMatrix *matrix, size_t size);
void matrix_put(struct MapMatrix *matrix, size_t row, size_t col, MapMatrixValue value);
MapMatrixValue matrix_get(struct MapMatrix *matrix, size_t row, size_t col);
void matrix_destroy(struct MapMatrix *matrix);

struct String {
    char *text;
    int text_len;
    int text_allocated;
};


void string_read_fd(FILE *fd, struct String *string);
void string_read_line(FILE *fd, struct String *string);

void string_add_symbol(struct String *string, char ch);
void string_reset(struct String *string);
void string_destroy(struct String *string);

bool
binary_to_hex(const uint8_t *binary, size_t binary_size, char *hex_text, size_t *hex_text_size);

#if defined(__cplusplus)
}
#endif

#endif //C_FSM_COMMON_H
