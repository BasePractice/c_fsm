// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "ant.h"
#include "sequence.h"

#define SEQUENCE "3.0.0.1:3.13.0.2:3.0.0.3:3.14.0.12:3.2.3.7:3.2.2.13:3.9.2.16:3.8.0.11:3.7.1.15:3.10.3.2:3.12.3.13:3.5.1.6:3.6.2.0:3.7.2.6:3.14.2.4:3.13.0.12:3.9.1.15"

int main(int argc, char **argv) {
    struct Sequence sequence = {0};
    char *text;
    int eq;

    sequence_create(&sequence, SEQUENCE);
    text = sequence_to_string(&sequence);
    eq = strcmp(text, SEQUENCE);
    ant_free(text);
    sequence_uml_fd(stdout, &sequence);
    sequence_format(&sequence, FormatLinks);
    sequence_uml_fd(stdout, &sequence);
    text = sequence_to_string(&sequence);
    fprintf(stdout, "Origin text : %s\n", SEQUENCE);
    fprintf(stdout, "Format text : %s\n", text);
    sequence_destroy(&sequence);
    return eq == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
