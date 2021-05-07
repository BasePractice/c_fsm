#include <stdio.h>
#include <stdlib.h>
#include "forth.h"

int
main(int argc, char **argv) {
    struct Fm *vm = fm_create();
    struct Entry *sum, *dot, *crlf, *base_r;

    fm_inter(vm, "1 BASE_W");
    fm_inter(vm, ": SUM ( a b -- c ) 10 60 + .\" HELLO\n\" ;");
    fm_inter(vm, ": PRINT 0 OUT ;");
    fm_inter(vm, ": CRLF 10 PRINT ;");
    fm_inter(vm, "SUM . .");
    fm_inter(vm, "CRLF");
    fm_inter(vm, "BASE_R .");

    sum = fm_search(vm, "SUM");
    dot = fm_search(vm, ".");
    crlf = fm_search(vm, "CRLF");
    base_r = fm_search(vm, "BASE_R");

    fprintf(stdout, "\n");

    fm_exec(vm, sum);
    fm_exec(vm, dot);
    fm_exec(vm, dot);
    fm_exec(vm, crlf);
    fm_exec(vm, base_r);
    fm_exec(vm, dot);

    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}

