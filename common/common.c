#include <stdlib.h>
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
