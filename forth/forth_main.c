// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include "forth.h"

static void memo_write_fd(u8 *ptr, u32 size, FILE *fd) {
    fwrite(ptr, 1, size, fd);
}

static u8 memo_read_fd(u32 it, FILE *fd) {
    u8 d;

    fseek(fd, it, SEEK_SET);
    fread(&d, 1, 1, fd);
    return d;
}

static int memo_eof_fd(u32 it, FILE *fd) {
    long v, t = ftell(fd);
    fseek(fd, 0, SEEK_END);
    v = ftell(fd);
    fseek(fd, t, SEEK_SET);
    return it >= v;
}

int
main(int argc, char **argv) {
    struct VM vm;
    struct Entry *sum, *dot, *crlf, *base_r;

    vm_create(&vm);
    vm_inter(&vm, "1 BASE_W");
    vm_inter(&vm, ": SUM ( a b -- c ) 10 60 + .\" HELLO\n\" ;");
    vm_inter(&vm, ": PRINT 0 OUT ;");
    vm_inter(&vm, ": CRLF 10 PRINT ;");
    vm_inter(&vm, "SUM . .");
    vm_inter(&vm, "CRLF");
    vm_inter(&vm, "BASE_R .");

    sum = vm_search(&vm, "SUM");
    dot = vm_search(&vm, ".");
    crlf = vm_search(&vm, "CRLF");
    base_r = vm_search(&vm, "BASE_R");

    fprintf(stdout, "\n");

    vm_exec(&vm, sum);
    vm_exec(&vm, dot);
    vm_exec(&vm, dot);
    vm_exec(&vm, crlf);
    vm_exec(&vm, base_r);
    vm_exec(&vm, dot);

    {
        FILE *fd = fopen("vm.mem", "wb");
        if (0 != fd) {
            vm_memo_read(&vm, (write_to) memo_write_fd, fd);
            fclose(fd);
        }
        fd = fopen("vm.mem", "rb");
        if (0 != fd) {
            vm_memo_write(&vm, (read_to) memo_read_fd, (eof_to) memo_eof_fd, fd);
            fclose(fd);
        }
    }

    fprintf(stdout, "\n");

    vm_exec(&vm, sum);
    vm_exec(&vm, dot);
    vm_exec(&vm, dot);
    vm_exec(&vm, crlf);
    vm_exec(&vm, base_r);
    vm_exec(&vm, dot);

    fprintf(stdout, "\n");

    return EXIT_SUCCESS;
}

