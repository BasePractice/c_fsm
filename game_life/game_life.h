#ifndef C_FSM_GAME_LIFE_H
#define C_FSM_GAME_LIFE_H
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

enum CellState {
    ALIVE, DEAD
};

enum WorldState {
    INIT, CALCULATE, VERIFY, COPY_PREVIOUS, QUIT
};

struct Cell {
    int x;
    int y;
    enum CellState previous_state;
    enum CellState current_state;
};

struct World {
    struct Cell **cells;
    size_t width;
    size_t height;
    enum WorldState state;
    bool running;
    FILE *output;
};

void init_world(struct World *w, size_t width, size_t height, const char *filename);
void poll_world(struct World *w);

#endif //C_FSM_GAME_LIFE_H
