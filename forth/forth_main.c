#include <stdio.h>
#include <stdlib.h>
#include "forth.h"

int
main(int argc, char **argv) {
    struct Fm *fm = fm_create();

    fm_inter(fm, "1 BASE_W");
    fm_inter(fm, ": SUM ( a b -- c ) 10 60 + .\" HELLO\n\" ;");
    fm_inter(fm, ": PRINT 0 OUT ;");
    fm_inter(fm, ": CRLF 13 PRINT ;");
    fm_inter(fm, "SUM . .");
    fm_inter(fm, "CRLF");
    fm_inter(fm, "BASE_R .");
    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}

