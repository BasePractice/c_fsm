// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/**
 (0[48]|1[2468][048]|2[13579][26])|
 ((3[2468][048]|4[3579][26])00)|
 ([5-6]|7\d|8[0-8])(\/|-|\.)(([1-9])|(0[0-2]))4((9[6-9]|[2-8]\d)?1\d{2})
**/

static char input_text[] = "5/00421100";
int input_it = 0;
int input_len = sizeof(input_text) / sizeof(input_text[0]);

enum State {
    START,

    GR1_1,
    GR1_2, GR1_3,
    GR1_4, GR1_5,

    GR2_1, GR2_2,
    GR2_3, GR2_4,
    GR2_5, GR2_6,

    GR3_1, GR3_2, GR3_3,
    GR3_4, GR3_5, GR3_6,
    GR3_7, GR3_8, GR3_9, GR3_10,
    GR3_11,

    COMPLETE,
    ERROR
};
enum State state = START;

static char read_next_symbol() {
    if (input_it >= input_len)
        return 0;
    return input_text[input_it++];
}

static bool main_automate(char symbol) {
    bool ret = false;
    switch (state) {
        case START: {
            if ('0' == symbol) {
                state = GR1_1;
            } else if ('1' == symbol) {
                state = GR1_2;
            } else if ('2' == symbol) {
                state = GR1_4;
            } else if ('3' == symbol) {
                state = GR2_1;
            } else if ('4' == symbol) {
                state = GR2_3;
            } else if ('5' == symbol) {
                state = GR3_1;
            } else if ('6' == symbol) {
                state = GR3_1;
            } else if ('7' == symbol) {
                state = GR3_2;
            } else if ('8' == symbol) {
                state = GR3_3;
            } else {
                state = ERROR;
            }
            break;
        }

        case GR1_1: {
            if ('4' == symbol || '8' == symbol) {
                state = COMPLETE;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR1_2: {
            if ('2' == symbol || '4' == symbol || '6' == symbol || '8' == symbol) {
                state = GR1_3;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR1_3: {
            if ('0' == symbol || '4' == symbol || '8' == symbol) {
                state = COMPLETE;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR1_4: {
            if ('1' == symbol || '3' == symbol || '5' == symbol || '7' == symbol || '9' == symbol) {
                state = GR1_5;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR1_5: {
            if ('2' == symbol || '6' == symbol) {
                state = COMPLETE;
            } else {
                state = ERROR;
            }
            break;
        }

        case GR2_1: {
            if ('2' == symbol || '4' == symbol || '6' == symbol || '8' == symbol) {
                state = GR2_2;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR2_2: {
            if ('0' == symbol || '4' == symbol || '8' == symbol) {
                state = GR2_5;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR2_3: {
            if ('3' == symbol || '5' == symbol || '7' == symbol || '9' == symbol) {
                state = GR2_4;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR2_4: {
            if ('6' == symbol || '2' == symbol) {
                state = GR2_5;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR2_5: {
            if ('0' == symbol) {
                state = GR2_6;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR2_6: {
            if ('0' == symbol) {
                state = COMPLETE;
            } else {
                state = ERROR;
            }
            break;
        }

        case GR3_1: {
            if ('/' == symbol || '-' == symbol || '.' == symbol) {
                state = GR3_4;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_2: {
            if (isdigit((int) symbol)) {
                state = GR3_1;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_3: {
            if ('0' <= symbol && '8' >= symbol) {
                state = GR3_1;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_4: {
            if ('0' == symbol) {
                state = GR3_6;
            } else if ('1' <= symbol && '9' >= symbol) {
                state = GR3_5;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_5: {
            if ('4' == symbol) {
                state = GR3_7;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_6: {
            if ('0' <= symbol && '2' >= symbol) {
                state = GR3_5;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_7: {
            if ('9' == symbol) {
                state = GR3_8;
            } else if ('1' == symbol) {
                state = GR3_9;
            } else if ('2' <= symbol && '8' >= symbol) {
                state = GR3_10;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_8: {
            if ('6' <= symbol && '9' >= symbol) {
                state = GR3_9;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_9: {
            if ('0' <= symbol && '9' >= symbol) {
                state = GR3_11;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_10: {
            if ('0' <= symbol && '9' >= symbol) {
                state = GR3_9;
            } else {
                state = ERROR;
            }
            break;
        }
        case GR3_11: {
            if ('0' <= symbol && '9' >= symbol) {
                state = COMPLETE;
            } else {
                state = ERROR;
            }
            break;
        }

        case COMPLETE: {
            ret = true;
            break;
        }
        default:
        case ERROR: {
            ret = false;
            break;
        }
    }
    return ret;
}

int main(int argc, char **argv) {
    int success = false;
    while (state != COMPLETE && state != ERROR) {
        char next_symbol = read_next_symbol();
        if (0 == next_symbol) {
            break;
        }
        fprintf(stdout, "%c", next_symbol);
        main_automate(next_symbol);
    }
    success = state == COMPLETE;
    fprintf(stdout, " ===> %s\n", success ? "success" : "failure");
    return EXIT_SUCCESS;
}
