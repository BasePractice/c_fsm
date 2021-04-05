#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <ctype.h>
#include "fsm_coin.h"

enum Output coin_machine_tick(struct CoinMachine *machine, enum Input input) {
    enum Output output = COUNT_0;
    switch (machine->state) {
        case STATE_0: {
            if (input == COIN_3) {
                output = COUNT_1;
                machine->state = STATE_0;
            } else if (input == COIN_1) {
                output = COUNT_0;
                machine->state = STATE_1;
            } else if (input == COIN_10) {
                output = COUNT_3;
                machine->state = STATE_1;
            } else if (input == COIN_5) {
                output = COUNT_1;
                machine->state = STATE_2;
            } else {
                output = COUNT_0;
                machine->state = STATE_E;
            }
            break;
        }
        case STATE_1: {
            if (input == COIN_3) {
                output = COUNT_1;
                machine->state = STATE_1;
            } else if (input == COIN_1) {
                output = COUNT_0;
                machine->state = STATE_2;
            } else if (input == COIN_10) {
                output = COUNT_3;
                machine->state = STATE_2;
            } else if (input == COIN_5) {
                output = COUNT_2;
                machine->state = STATE_0;
            } else {
                output = COUNT_0;
                machine->state = STATE_E;
            }
            break;
        }
        case STATE_2: {
            if (input == COIN_3) {
                output = COUNT_1;
                machine->state = STATE_2;
            } else if (input == COIN_1) {
                output = COUNT_1;
                machine->state = STATE_0;
            } else if (input == COIN_10) {
                output = COUNT_4;
                machine->state = STATE_0;
            } else if (input == COIN_5) {
                output = COUNT_2;
                machine->state = STATE_1;
            } else {
                output = COUNT_0;
                machine->state = STATE_E;
            }
            break;
        }
        case STATE_E: {
            fprintf(stderr, "Error mode\n");
            machine->state = STATE_E;
            break;
        }
    }
    return output;
}
