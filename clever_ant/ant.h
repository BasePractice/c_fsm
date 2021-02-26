#ifndef C_FSM_ANT_H
#define C_FSM_ANT_H
#include <stdint.h>

#define SQUARE_SIZE 32

#define SQUARE_EMPTY 0
#define SQUARE_APPLE 1
#define SQUARE_PATH  2

#define DIRECT_UP      1
#define DIRECT_RIGHT   2
#define DIRECT_BOTTOM  3
#define DIRECT_LEFT    4

extern const uint8_t APPLES;
extern const uint8_t TORUS[SQUARE_SIZE][SQUARE_SIZE];

#endif //C_FSM_ANT_H
