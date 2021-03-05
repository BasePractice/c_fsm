#include <stdio.h>
#include <stdlib.h>
#include "forth.h"

int
main(int argc, char **argv) {
    struct Fm *fm = fm_create();

    fm_compile(fm, ": SUM ( a b -- c ) 10 60 + .\" HELLO\n\" ; SUM . . ");
    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}
