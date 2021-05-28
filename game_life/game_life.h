// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
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
    PRESET, INIT, CALCULATE, VERIFY, COPY_PREVIOUS, QUIT
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
void preset_world(struct World *w, size_t preset_width, struct Cell *cell, size_t cell_size);
void poll_world(struct World *w);

#endif //C_FSM_GAME_LIFE_H
