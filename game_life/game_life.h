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
    int value;
};

struct World {
    struct Cell **cells;
    size_t width;
    size_t height;
    enum WorldState state;
    bool running;
    FILE *output;
    bool output_cells;
};

void init_world(struct World *w, size_t width, size_t height, const char *filename, int value);
void destroy_world(struct World *w);
void preset_world(struct World *w, size_t preset_width, struct Cell *cell, size_t cell_size, int value);
void poll_world(struct World *w);
void clean_world(struct World *w, int value);
void toggle_live_world(struct World *w, size_t x, size_t y, int value);
void print_world(struct World *w);
struct Cell *cell(struct World *w, size_t x, size_t y);

#endif //C_FSM_GAME_LIFE_H
