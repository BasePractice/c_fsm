// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <memory.h>
#include <string.h>

/**
 1. 3664600, 36752124600, 367535124600, 368508124600

(
 (36[0-8](5[0-3][567]?9?8?12)?46)
 |
 (12[67]?92[0-9]10*36?)
 |
 ((80[67]36?5)?76?5?89(10|11|12)?26)
)00
**/

enum RegexpState {
    STATE_BEGIN,

    STATE_K,
    STATE_K_1,

    STATE_S1_1, STATE_S1_2, STATE_S1_3, STATE_S1_4, STATE_S1_5, STATE_S1_6,
    STATE_S1_7, STATE_S1_8, STATE_S1_9, STATE_S1_10, STATE_S1_11,

    STATE_S2_1, STATE_S2_2, STATE_S2_3, STATE_S2_4, STATE_S2_5, STATE_S2_6,
    STATE_S2_7, STATE_S2_8,

    STATE_ERROR, STATE_END
};

struct Regexp {
    enum RegexpState state;
    char *text;
    size_t text_len;
    int text_it;
};

static void regexp_init(struct Regexp *regexp, char *text) {
    memset(regexp, 0, sizeof(struct Regexp));
    regexp->state = STATE_BEGIN;
    regexp->text = strdup(text);
    regexp->text_len = strlen(text);
}

static void regexp_destroy(struct Regexp *regexp) {
    if (0 != regexp) {
        if (0 != regexp->text) {
            free(regexp->text);
        }
        regexp->text = 0;
        regexp->text_len = 0;
        regexp->text_it = 0;
    }
}

static void regexp_do(struct Regexp *regexp, char ch) {
    switch (regexp->state) {
        case STATE_BEGIN: {
            if ('3' == ch) {
                regexp->state = STATE_S1_1;
            } else if ('1' == ch) {
                regexp->state = STATE_S2_1;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_K: {
            if ('0' == ch) {
                regexp->state = STATE_K_1;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_K_1: {
            if ('0' == ch) {
                regexp->state = STATE_END;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }

        /*1-ая группа*/

        case STATE_S1_1: {
            if ('6' == ch) {
                regexp->state = STATE_S1_2;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_2: {
            if ('0' <= ch && ch <= '8') {
                regexp->state = STATE_S1_3;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_3: {
            if ('4' == ch) {
                regexp->state = STATE_S1_5;
            } else if ('5' == ch) {
                regexp->state = STATE_S1_4;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_4: {
            if ('0' <= ch && ch <= '3') {
                regexp->state = STATE_S1_6;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_5: {
            if ('6' == ch) {
                regexp->state = STATE_K;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_6: {
            if ('5' <= ch && ch <= '7') {
                regexp->state = STATE_S1_7;
            } else if ('9' == ch) {
                regexp->state = STATE_S1_8;
            } else if ('8' == ch) {
                regexp->state = STATE_S1_9;
            } else if ('1' == ch) {
                regexp->state = STATE_S1_10;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_7: {
            if ('9' == ch) {
                regexp->state = STATE_S1_8;
            } else if ('8' == ch) {
                regexp->state = STATE_S1_9;
            } else if ('1' == ch) {
                regexp->state = STATE_S1_10;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_8: {
            if ('8' == ch) {
                regexp->state = STATE_S1_9;
            } else if ('1' == ch) {
                regexp->state = STATE_S1_10;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_9: {
            if ('1' == ch) {
                regexp->state = STATE_S1_10;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_10: {
            if ('2' == ch) {
                regexp->state = STATE_S1_11;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S1_11: {
            if ('4' == ch) {
                regexp->state = STATE_S1_5;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }

        /*2-ая группа*/

        case STATE_S2_1: {
            if ('2' == ch) {
                regexp->state = STATE_S2_2;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_2: {
            if ('6' == ch || '7' == ch) {
                regexp->state = STATE_S2_4;
            } else if ('9' == ch) {
                regexp->state = STATE_S2_3;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_3: {
            if ('2' == ch) {
                regexp->state = STATE_S2_5;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_4: {
            if ('9' == ch) {
                regexp->state = STATE_S2_3;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_5: {
            if ('0' <= ch && '9' >= ch) {
                regexp->state = STATE_S2_6;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_6: {
            if ('1' == ch) {
                regexp->state = STATE_S2_7;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_7: {
            if ('0' == ch) {
//                regexp->state = STATE_S2_7;
            } else if ('3' == ch) {
                regexp->state = STATE_S2_8;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }
        case STATE_S2_8: {
            if ('0' == ch) {
                regexp->state = STATE_K_1;
            } else if ('6' == ch) {
                regexp->state = STATE_K;
            } else {
                regexp->state = STATE_ERROR;
            }
            break;
        }

        /*3-я группа*/

        default:
        case STATE_END:
        case STATE_ERROR: {
            break;
        }
    }
}

struct {
    char *text;
    bool result;
} tests[] = {
        {"3664600",      true},
        {"36752124600",  true},
        {"367535124600", true},
        {"368508124600", true},
        {"3685124600",   false},
};

static bool regexp_execute(struct Regexp *regexp) {
    bool running = true;
    bool result = false;
    while (running) {
        regexp_do(regexp, regexp->text[regexp->text_it]);
        regexp->text_it++;
        if (STATE_ERROR == regexp->state) {
            running = false;
        } else if (STATE_END == regexp->state) {
            result = true;
            running = false;
        } else if (regexp->text_it >= regexp->text_len) {
            running = false;
        }
    }
    return result;
}

int main(int argc, char **argv) {
    struct Regexp regexp = {STATE_BEGIN, 0, 0, 0};
    int i;
    bool result;

    for (i = 0; i < sizeof(tests) / sizeof(tests[0]); ++i) {
        regexp_init(&regexp, tests[i].text);
        result = regexp_execute(&regexp);
        fprintf(stdout, "%20s[%4s]: %s\n",
                regexp.text, tests[i].result ? "GOOD" : "BAD", result == tests[i].result ? "SUCCESS" : "FAILURE");
        fflush(stdout);
        regexp_destroy(&regexp);
    }
    return EXIT_SUCCESS;
}
