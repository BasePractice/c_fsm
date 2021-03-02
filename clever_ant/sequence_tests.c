#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sequence.h"

#define SEQUENCE "3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.9.2.19:3.8.0.11:3.7.1.17:3.10.3.2:3.8.3.4:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12:3.18.0.2:3.9.1.17"

int main(int argc, char **argv) {
    struct Sequence sequence = {0};
    char *text;
    int eq;

    sequence_create(&sequence, SEQUENCE);
    text = sequence_to_string(&sequence);
    eq = strcmp(text, SEQUENCE);
    sequence_destroy(&sequence);
    return eq == 0 ? EXIT_SUCCESS : EXIT_FAILURE;
}
