// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include "common.h"

#define STR_LINES "LINE1\nLINE2\nLINE3"

int main(int argc, char **argv) {
    struct String str = {0, 0, 0};
    FILE *fd;

    fd = fopen("simulate.common_string.txt", "rt");
    if (0 != fd) {
        string_reset(&str);
        string_read_fd(fd, &str);
        fprintf(stdout, "LINES: %s\n", strncmp(str.text, STR_LINES, str.text_len) ? "false" : "true");
        fflush(stdout);
        fclose(fd);
    }

    fd = fopen("simulate.common_string.txt", "rt");
    if (0 != fd) {
        bool next = true;

        while (next) {
            string_reset(&str);
            string_read_line(fd, &str);
            if (str.text_len <= 0) {
                next = false;
                continue;
            }
            fprintf(stdout, "LINE : %.*s\n", str.text_len, str.text);
            fflush(stdout);
        }
        fclose(fd);
    }

    fprintf(stdout, "INPUT>\n");
    fflush(stdout);
    while (str.text == 0 || strncmp("end", str.text, 3) != 0) {
        string_reset(&str);
        string_read_line(stdin, &str);
        fprintf(stdout, "INPUT: %.*s\n", str.text_len, str.text);
        fflush(stdout);
    }
    return EXIT_SUCCESS;
}

