// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "fsm_bulk.h"

static int test() {
    uint8_t sequence[4] = {0, 0, 0, 0};
    int i, k;

    read_sequence(sequence, 334995432);
    if (sequence[0] != 5 && sequence[1] != 4 && sequence[2] != 3 && sequence[3] != 2)
        return EXIT_FAILURE;
    for (k = 0; k < 4; ++k) {
        if (k > 0)
            fprintf(stdout, ".");
        for (i = 2; i >= 0; --i) {
            fprintf(stdout, "%s", READ_PART(sequence[k], i) == true ? "1" : "0");
        }
    }
    fprintf(stdout, "\n");
    return EXIT_SUCCESS;
}

int main(int argc, char **argv) {
    struct FSM *fsm = fsm_create("simulate.bulk.t", 5151, true);
    do {
        fsm_update(fsm);
        fsm_tick(fsm);
    } while (fsm_eof(fsm) == false);
    fsm_destroy(&fsm);
    return EXIT_SUCCESS;
}