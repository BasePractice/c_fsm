#include <stdio.h>
#include <stdlib.h>
#include "forth.h"

static void memo_write_fd(u8 *ptr, u32 size, FILE *fd) {
    fwrite(ptr, size, 1, fd);
}

static u8 memo_read_fd(u32 it, FILE *fd) {
    u8 d;

    fseek(fd, it, SEEK_SET);
    fread(&d, 1, 1, fd);
    return d;
}

static int memo_eof_fd(u32 it, FILE *fd) {
    fseek(fd, it, SEEK_SET);
    return feof(fd);
}

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

    {
        FILE *fd = fopen("vm.mem", "w");
        if (0 != fd) {
            fm_memo_read(vm, (write_to) memo_write_fd, fd);
            fclose(fd);
        }
    }

    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}

