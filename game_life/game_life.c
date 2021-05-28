// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include "game_life.h"

void init_world(struct World *w, size_t width, size_t height, const char *filename) {
    size_t i;

    w->height = height;
    w->width = width;
    w->cells = (struct Cell **) malloc(sizeof(struct Cell *) * width);
    for (i = 0; i < width; ++i) {
        w->cells[i] = (struct Cell *) malloc(sizeof(struct Cell) * height);
    }
    w->state = INIT;
    w->running = true;
    if (filename == 0 || (w->output = fopen(filename, "w")) == 0) {
        w->output = 0;
    }
}

void preset_world(struct World *w, size_t preset_width, struct Cell *cell, size_t cell_size) {
    size_t i, x_offset = 0, y_offset = 0;
    size_t k;


    x_offset = w->width / 2 - preset_width / 2;
    y_offset = w->height / 2 - preset_width / 2;
    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            w->cells[i][k].current_state = w->cells[i][k].previous_state = DEAD;
            w->cells[i][k].x = i;
            w->cells[i][k].y = k;
        }
    }

    for (i = 0; i < cell_size; ++i) {
        w->cells[cell[i].x + x_offset][cell[i].y + y_offset].current_state
                = w->cells[cell[i].x + x_offset][cell[i].y + y_offset].previous_state = ALIVE;
        w->cells[cell[i].x + x_offset][cell[i].y + y_offset].x = cell[i].x + x_offset;
        w->cells[cell[i].x + x_offset][cell[i].y + y_offset].y = cell[i].y + y_offset;
    }

    w->state = PRESET;
}

static void fill_world(struct World *w) {
    size_t i;
    size_t k;

    srand((unsigned int) time(0));
    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            w->cells[i][k].current_state = w->cells[i][k].previous_state = ((rand() & 1) == 0 ? ALIVE : DEAD);
            w->cells[i][k].x = i;
            w->cells[i][k].y = k;
        }
    }
}

static void print_world(struct World *w) {
    size_t i;
    size_t k;

    if (0 == w->output)
        return;

    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            if (k > 0)
                fprintf(w->output, " ");
            fprintf(w->output, "%c", w->cells[i][k].current_state == DEAD ? ' ' : '*');
        }
        fprintf(w->output, "\n");
    }
    for (k = 0; k < w->width; ++k) {
        fprintf(w->output, "- ");
    }
    fprintf(w->output, "\n");
    fflush(w->output);
}

static int around_the_cell(struct World *w, struct Cell *cell, struct Cell ***result) {
    static struct Cell *not_thread_safe[8];
    int ret = 0;
    int ity;
    int itx;
    size_t iy;
    size_t ix;


    (*result) = not_thread_safe;
    for (itx = cell->x - 1; itx <= cell->x + 1; ++itx) {
        for (ity = cell->y - 1; ity <= cell->y + 1; ++ity) {
            iy = ity;
            ix = itx;
            if (iy == cell->y && ix == cell->x)
                continue;

            if (iy < 0) {
                iy = w->height - 1;
            } else if (iy >= w->height) {
                iy = 0;
            }

            if (ix < 0) {
                ix = w->width - 1;
            } else if (ix >= w->width) {
                ix = 0;
            }

            not_thread_safe[ret] = &w->cells[ix][iy];
            ++ret;
        }
    }
    return ret;
}

static inline void do_cell_automate(struct Cell *c, int d) {
    if (c->previous_state == DEAD && d == 3) {
        c->current_state = ALIVE;
    } else {
        if (c->previous_state == ALIVE && (d == 3 || d == 2)) {
            c->current_state = ALIVE;
        } else {
            c->current_state = DEAD;
        }
    }
}

static void regenerate_world(struct World *w) {
    size_t i;
    size_t k;

    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            struct Cell **result = 0;
            int count = around_the_cell(w, &w->cells[i][k], &result);
            int alive = 0;
            int d;

            for (d = 0; d < count; ++d) {
                if (result[d]->previous_state == ALIVE) {
                    alive += 1;
                }
            }

            do_cell_automate(&w->cells[i][k], alive);
        }
    }
}

static void copy_world(struct World *w) {
    size_t i;
    size_t k;

    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            w->cells[i][k].previous_state = w->cells[i][k].current_state;
        }
    }
}

static bool equals_world(struct World *w) {
    size_t i;
    size_t k;

    for (i = 0; i < w->width; ++i) {
        for (k = 0; k < w->height; ++k) {
            if (w->cells[i][k].current_state != w->cells[i][k].previous_state)
                return false;
        }
    }
    return true;
}

void poll_world(struct World *w) {
    switch (w->state) {
        case PRESET: {
            w->state = CALCULATE;
            break;
        }
        case INIT: {
            fill_world(w);
            print_world(w);
            w->state = CALCULATE;
            break;
        }
        case CALCULATE: {
            regenerate_world(w);
            print_world(w);
            w->state = VERIFY;
            break;
        }
        case QUIT: {
            w->running = false;
            break;
        }

        case VERIFY: {
            w->state = equals_world(w) ? QUIT : COPY_PREVIOUS;
            break;
        }

        case COPY_PREVIOUS: {
            copy_world(w);
            w->state = CALCULATE;
            break;
        }
    }
}
