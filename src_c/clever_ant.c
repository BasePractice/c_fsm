#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>

#define SQUARE_SIZE 32

#define SQUARE_EMPTY 0
#define SQUARE_APPLE 1
#define SQUARE_PATH  2

#define DIRECT_UP      1
#define DIRECT_RIGHT   2
#define DIRECT_BOTTOM  3
#define DIRECT_LEFT    4

#define STATE_1    0
#define STATE_2    1
#define STATE_3    2
#define STATE_4    3
#define STATE_5    4

static uint8_t torus[SQUARE_SIZE][SQUARE_SIZE] = {
        {0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 00
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 01
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 02
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 03
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 04
        {1, 1, 1, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 1, 1}, // 05
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 06
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 07
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 2, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 08
        {0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 09
        {0, 0, 0, 1, 1, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0}, // 10
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 2, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0}, // 11
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 2, 2, 1, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 12
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 13
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 14
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 2, 2, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 15
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 16
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 17
        {0, 0, 0, 0, 0, 0, 0, 2, 1, 2, 2, 2, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 18
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 19
        {0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 20
        {0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 21
        {0, 0, 0, 0, 2, 1, 2, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 22
        {0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 23
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 24
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 25
        {0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 26
        {0, 0, 0, 0, 1, 2, 2, 1, 1, 1, 1, 2, 1, 1, 1, 1, 1, 1, 2, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 27
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 28
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 29
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}, // 30
        {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0}  // 31
};
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

static void turn_left() {
    --ant_direct;
    if (ant_direct < DIRECT_UP)
        ant_direct = DIRECT_LEFT;
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
    return 0;
}