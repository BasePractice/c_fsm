#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>
#include "fsm_coin.h"

static int read_coin(FILE *in) {
    char buf[3] = {0};
    int buf_i = 0;
    int ch, d;

    next:
    fprintf(stdout, "Enter coin> ");
    fflush(stdout);
    while (true) {
        ch = fgetc(in);
        if ('\n' == ch) {
            break;
        } else if (!isdigit(ch) && buf_i + 1 > 3) {
            fprintf(stderr, "Error: non digit input or more symbols\n");
            fflush(stderr);
            buf_i = 0;
            fflush(in);
            continue;
        }
        buf[buf_i++] = ch;
    }
    buf[buf_i] = 0;
    d = (int) strtol(buf, 0, 10);
    if (d == 1 || d == 3 || d == 5 || d == 10)
        return d;
    fprintf(stderr, "Only 1, 3, 5, 10 must be enter\n");
    fflush(stderr);
    buf_i = 0;
    goto next;
}

int main(int argc, char **argv) {
    struct CoinMachine machine = {STATE_0};
    while (true) {
        int d = read_coin(stdin);
        enum Output output = coin_machine_tick(&machine, (enum Input) d);
        fprintf(stdout, "Machine   > [%2d, %d, State = %d]\n", d, (int) output, (int) machine.state);
    }
    return EXIT_SUCCESS;
}

