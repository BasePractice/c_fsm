#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <ctype.h>

/**
 (ISBN(:?))?((97[89])?\d{9}[\dx])
**/

static char input_text[] = "ISBN:979000000000x";
int input_it = 0;
int input_len = sizeof(input_text) / sizeof(input_text[0]);

enum State {
    START,

    EAT_I, EAT_S, EAT_B, EAT_N, EAT_COLON,
    EAT_9_DIGIT, EAT_7_DIGIT, EAT_OR_9_DIGIT, EAT_OR_8_DIGIT,

    NEXT_8_DIGITS, NEXT_7_DIGITS, NEXT_6_DIGITS, NEXT_5_DIGITS,
    NEXT_4_DIGITS, NEXT_3_DIGITS, NEXT_2_DIGITS, NEXT_1_DIGITS, NEXT_OR,

    COMPLETE
};
enum State state = START;


static char read_next_symbol() {
    if (input_it >= input_len)
        return 0;
    return input_text[input_it++];
}

static bool main_automate(char symbol) {
    switch (state) {
        case START: {
            if (symbol == 'I') {
                state = EAT_I;
                return true;
            } else if (symbol == '9') {
                state = EAT_9_DIGIT;
                return true;
            } else if (true == isdigit(symbol)) {
                state = NEXT_8_DIGITS;
                return true;
            }
            break;
        }
        case EAT_I: {
            if (symbol == 'S') {
                state = EAT_S;
                return true;
            }
            break;
        }
        case EAT_S: {
            if (symbol == 'B') {
                state = EAT_B;
                return true;
            }
            break;
        }
        case EAT_B: {
            if (symbol == 'N') {
                state = EAT_N;
                return true;
            }
            break;
        }
        case EAT_N: {
            if (symbol == ':') {
                state = EAT_COLON;
                return true;
            } else if (symbol == '9') {
                state = EAT_9_DIGIT;
                return true;
            } else if (true == isdigit(symbol)) {
                state = NEXT_8_DIGITS;
                return true;
            }
            break;
        }
        case EAT_COLON: {
            if (symbol == '9') {
                state = EAT_9_DIGIT;
                return true;
            } else if (true == isdigit(symbol)) {
                state = NEXT_8_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_8_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_7_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_7_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_6_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_6_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_5_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_5_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_4_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_4_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_3_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_3_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_2_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_2_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_1_DIGITS;
                return true;
            }
            break;
        }
        case NEXT_1_DIGITS: {
            if (true == isdigit(symbol)) {
                state = NEXT_OR;
                return true;
            }
            break;
        }
        case NEXT_OR: {
            if (true == isdigit(symbol)) {
                state = COMPLETE;
                return true;
            } else if (symbol == 'x') {
                state = COMPLETE;
                return true;
            }
            break;
        }
        case EAT_9_DIGIT: {
            if (symbol == '7') {
                state = EAT_7_DIGIT;
                return true;
            } else if (true == isdigit(symbol)) {
                state = NEXT_7_DIGITS;
                return true;
            }
            break;
        }
        case EAT_7_DIGIT: {
            if (symbol == '8') {
                state = EAT_OR_8_DIGIT;
                return true;
            } else if (symbol == '9') {
                state = EAT_OR_9_DIGIT;
                return true;
            } else if (true == isdigit(symbol)) {
                state = NEXT_6_DIGITS;
                return true;
            }
        }
        case EAT_OR_9_DIGIT:
        case EAT_OR_8_DIGIT: {
            if (true == isdigit(symbol)) {
                state = NEXT_8_DIGITS;
                return true;
            }
            break;
        }
        case COMPLETE:
            return 0 == symbol ? true : false;
        default:
            break;
    }
    return 0;
}

int main(int argc, char **argv) {
    int success = 0;
    while (1) {
        char next_symbol = read_next_symbol();
        if (0 == next_symbol) {
            success = state == COMPLETE;
            break;
        }
        fprintf(stdout, "%c", next_symbol);
        int ret = main_automate(next_symbol);
        if (0 == ret) {
            success = 0;
            break;
        }
    }
    fprintf(stdout, " ===> %s\n", success ? "success" : "failure");
    return EXIT_SUCCESS;
}
