// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>

/**
 * ^<(kill|pop)>.*<\/\1>$
 */

enum State {
    START,   /* Начальное состояние */

    TAG1_LEFT_BRICE, /*<*/
    TAG1_RIGHT_BRICE, /*>*/

    TAG1_LETTER_K,
    TAG1_LETTER_I,
    TAG1_LETTER_L1,
    TAG1_LETTER_L2,

    TAG1_LETTER_P1,
    TAG1_LETTER_O,
    TAG1_LETTER_P2,

    ANY_SYMBOLS, /* .* */

    TAG2_LEFT_BRICE, /*<*/
    TAG2_RIGHT_BRICE, /*>*/
    TAG2_RIGHT_SYMB, /*\/ */
    TAG2_MEMORY,

    COMPLETE,
    FAILURE, /* Ошибочное состояние*/
};

struct Engine {
    enum State state;
    int tag[4];
    int tag_size;
    int tag_it;
};

static void engine_reset(struct Engine *engine) {
    engine->state = START;
    engine->tag_size = 0;
    engine->tag_it = 0;
    memset(engine->tag, 0, sizeof(engine->tag));
}

static bool automate(struct Engine *engine, int character) {
    bool result = false;
    switch (engine->state) {
        case START: {
            if ('<' == character) {
                engine->state = TAG1_LEFT_BRICE;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LEFT_BRICE: {
            if ('p' == character) {
                engine->state = TAG1_LETTER_P1;
            } else if ('k' == character) {
                engine->state = TAG1_LETTER_K;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_RIGHT_BRICE: {
            if ('<' == character) {
                engine->state = TAG2_LEFT_BRICE;
            } else {
                engine->state = ANY_SYMBOLS;
            }
            break;
        }
        case TAG1_LETTER_K: {
            if ('i' == character) {
                engine->state = TAG1_LETTER_I;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LETTER_I: {
            if ('l' == character) {
                engine->state = TAG1_LETTER_L1;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LETTER_L1: {
            if ('l' == character) {
                engine->state = TAG1_LETTER_L2;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LETTER_L2: {
            engine->tag_size = 4;
            engine->tag[0] = 'k';
            engine->tag[1] = 'i';
            engine->tag[2] = 'l';
            engine->tag[3] = 'l';
            engine->tag_it = 0;
            engine->state = TAG1_RIGHT_BRICE;
            break;
        }
        case TAG1_LETTER_P1: {
            if ('o' == character) {
                engine->state = TAG1_LETTER_O;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LETTER_O: {
            if ('p' == character) {
                engine->state = TAG1_LETTER_P2;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG1_LETTER_P2: {
            engine->tag_size = 3;
            engine->tag[0] = 'p';
            engine->tag[1] = 'o';
            engine->tag[2] = 'p';
            engine->tag_it = 0;
            engine->state = TAG1_RIGHT_BRICE;
            break;
        }
        case ANY_SYMBOLS: {
            if ('<' == character) {
                engine->state = TAG2_LEFT_BRICE;
            }
            break;
        }
        case TAG2_LEFT_BRICE: {
            if ('/' == character) {
                engine->state = TAG2_RIGHT_SYMB;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG2_RIGHT_BRICE: {
            break;
        }
        case TAG2_RIGHT_SYMB: {
            if (engine->tag[engine->tag_it++] == character) {
                engine->state = TAG2_MEMORY;
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case TAG2_MEMORY: {
            if (engine->tag_it >= engine->tag_size) {
                if ('>' == character) {
                    engine->state = TAG1_RIGHT_BRICE;
                    result = true;
                } else {
                    engine->state = FAILURE;
                }
            } else if (engine->tag[engine->tag_it++] == character) {
                /*warning: V1048 The 'engine->state' variable was assigned the same value.*/
                /*engine->state = TAG2_MEMORY;*/
            } else {
                engine->state = FAILURE;
            }
            break;
        }
        case FAILURE: {
            break;
        }
        case COMPLETE: {
            result = character == 0 ? true : false;
            break;
        }
    }
    return result;
}

int main(int argc, char **argv) {
    char *input;
    int input_size;
    int input_it, it;
    bool result = false;
    bool running = true;
    struct Engine engine = {START};

    for (it = 0; it < argc; ++it) {
        if (it == 0) {
            input = "<kill>Some text</kill>";
        } else {
            input = argv[it];
        }
        input_size = strlen(input);
        input_it = 0;
        engine_reset(&engine);
        while (running) {
            if (input_it >= input_size)
                break;
            int character = input[input_it++];
            result = automate(&engine, character);
            running = engine.state != FAILURE;
        }
        running = true;
        fprintf(stdout, "%s[%d]: %s\n", input, input_it, result ? "SUCCESS" : "FAILURE");
    }
    return EXIT_SUCCESS;
}
