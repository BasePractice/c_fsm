// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_COMMON_H
#define C_FSM_COMMON_H

#include <stdio.h>

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

#endif //C_FSM_COMMON_H
