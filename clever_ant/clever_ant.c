#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "ant.h"

#define STATE_1    0
#define STATE_2    1
#define STATE_3    2
#define STATE_4    3
#define STATE_5    4

static uint8_t torus[SQUARE_SIZE][SQUARE_SIZE];
static int8_t ant_x = 0;
static int8_t ant_y = 0;
static uint8_t ant_state = STATE_1;
static uint8_t ant_direct = DIRECT_RIGHT;
static uint16_t steps = 0;
static uint16_t apples = 89;

static void eat() {
    if (torus[ant_x][ant_y] == SQUARE_APPLE) {
        --apples;
        torus[ant_x][ant_y] = SQUARE_PATH;
    }
}

static void step() {
    if (ant_direct == DIRECT_RIGHT) {
        ++ant_y;
        if (ant_y >= SQUARE_SIZE)
            ant_y = 0;
    } else if (ant_direct == DIRECT_LEFT) {
        --ant_y;
        if (ant_y < 0)
            ant_y = SQUARE_SIZE - 1;
    } else if (ant_direct == DIRECT_UP) {
        --ant_x;
        if (ant_x < 0)
            ant_x = SQUARE_SIZE - 1;
    } else if (ant_direct == DIRECT_BOTTOM) {
        ++ant_x;
        if (ant_x >= SQUARE_SIZE)
            ant_x = 0;
    }
}

static void turn_right() {
    ++ant_direct;
    if (ant_direct > DIRECT_LEFT)
        ant_direct = DIRECT_UP;
}

static bool has_food() {
    if (ant_direct == DIRECT_RIGHT) {
        if (ant_y + 1 >= SQUARE_SIZE)
            return torus[ant_x][0] == SQUARE_APPLE;
        return torus[ant_x][ant_y + 1] == SQUARE_APPLE;
    } else if (ant_direct == DIRECT_LEFT) {
        if (ant_y - 1 < 0)
            return torus[ant_x][SQUARE_SIZE - 1] == SQUARE_APPLE;
        return torus[ant_x][ant_y - 1] == SQUARE_APPLE;
    } else if (ant_direct == DIRECT_UP) {
        if (ant_x - 1 < 0)
            return torus[SQUARE_SIZE - 1][ant_y] == SQUARE_APPLE;
        return torus[ant_x - 1][ant_y] == SQUARE_APPLE;
    } else if (ant_direct == DIRECT_BOTTOM) {
        if (ant_x + 1 >= SQUARE_SIZE)
            return torus[0][ant_y] == SQUARE_APPLE;
        return torus[ant_x + 1][ant_y] == SQUARE_APPLE;
    }
    assert(false);
    return false;
}

/*http://www.demo.cs.brandeis.edu/papers/ep93.pdf */
int main() {
    size_t i, k;

    for (i = 0; i < SQUARE_SIZE; ++i) {
        for (k = 0; k < SQUARE_SIZE; ++k) {
            torus[i][k] = TORUS[i][k];
        }
    }
    while (apples > 0) {
        switch (ant_state) {
            case STATE_1:
                if (has_food()) {
                    step();
                    eat();
                } else {
                    turn_right();
                    ant_state = STATE_2;
                }
                break;
            case STATE_2:
                if (has_food()) {
                    step();
                    eat();
                    ant_state = STATE_1;
                } else {
                    turn_right();
                    ant_state = STATE_3;
                }
                break;
            case STATE_3:
                if (has_food()) {
                    step();
                    eat();
                    ant_state = STATE_1;
                } else {
                    turn_right();
                    ant_state = STATE_4;
                }
                break;
            case STATE_4:
                if (has_food()) {
                    step();
                    eat();
                    ant_state = STATE_1;
                } else {
                    turn_right();
                    ant_state = STATE_5;
                }
                break;
            case STATE_5:
                if (has_food()) {
                    step();
                    eat();
                } else {
                    step();
                }
                ant_state = STATE_1;
                break;
            default:
                assert(false);
                break;
        }
        ++steps;
    }
    fprintf(stdout, "Всего шагов: %d\n", steps);
    return 0;
}