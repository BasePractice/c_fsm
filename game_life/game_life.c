// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <assert.h>
#include "game_life.h"

struct Cell *cell(struct World *w, size_t x, size_t y) {
    assert(w != 0);
    assert(x >= 0 && x < w->width );
    assert(y >= 0 && y < w->height );
    struct Cell *p_cell = w->cells[x];
    return &p_cell[y];
}

void init_world(struct World *w, size_t width, size_t height, const char *filename, int value) {
    size_t x;
    size_t y;

    w->height = height;
    w->width = width;
    w->cells = (struct Cell **) malloc(sizeof(struct Cell *) * width);
    for (x = 0; x < width; ++x) {
        struct Cell *c = (struct Cell *) malloc(sizeof(struct Cell) * height);
        for (y = 0; y < height; ++y) {
            c[y].value = value;
            c[y].x = x;
            c[y].y = y;
        }
        w->cells[x] = c;
    }
    w->state = INIT;
    w->running = true;
    if (filename == 0 || (w->output = fopen(filename, "w")) == 0) {
        w->output = stdout;
    }
}

void preset_world(struct World *w, size_t preset_width, struct Cell *src, size_t cell_size, int value) {
    size_t i, x_offset = 0, y_offset = 0;
    size_t k;


    x_offset = w->width / 2 - preset_width / 2;
    y_offset = w->height / 2 - preset_width / 2;
    clean_world(w, value);
    for (i = 0; i < cell_size; ++i) {
        struct Cell *c = cell(w, src[i].x + x_offset, src[i].y + y_offset);
        c->current_state = c->previous_state = ALIVE;
        c->x = src[i].x + x_offset;
        c->y = src[i].y + y_offset;
        c->value = value;
    }

    w->state = PRESET;
}

static void fill_world(struct World *w) {
    size_t x;
    size_t y;

    srand((unsigned int) time(0));
    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            struct Cell *c = cell(w, x, y);
            c->current_state = c->previous_state = ((rand() & 1) == 0 ? ALIVE : DEAD);
            c->x = x;
            c->y = y;
        }
    }
}

void print_world(struct World *w) {
    size_t x;
    size_t y;

    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            if (y > 0)
                fprintf(w->output, " ");
            struct Cell *c = cell(w, x, y);
            fprintf(w->output, "%c", c->current_state == DEAD ? ' ' : '*');
        }
        fprintf(w->output, "\n");
    }
    for (y = 0; y < w->width; ++y) {
        fprintf(w->output, "- ");
    }
    fprintf(w->output, "\n");
    fflush(w->output);
}

static int around_the_cell(struct World *w, struct Cell *c, struct Cell ***result) {
    static struct Cell *not_thread_safe[8];
    int ret = 0;
    int ity;
    int itx;
    size_t iy;
    size_t ix;


    (*result) = not_thread_safe;
    for (itx = c->x - 1; itx <= c->x + 1; ++itx) {
        for (ity = c->y - 1; ity <= c->y + 1; ++ity) {
            iy = ity;
            ix = itx;
            if (iy == c->y && ix == c->x)
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
            not_thread_safe[ret] = cell(w, ix, iy);
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
    size_t x;
    size_t y;

    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            struct Cell **result = 0;
            struct Cell *c = cell(w, x, y);
            int count = around_the_cell(w, c, &result);
            int alive = 0;
            int d;

            for (d = 0; d < count; ++d) {
                if (result[d]->previous_state == ALIVE) {
                    alive += 1;
                }
            }

            do_cell_automate(c, alive);
        }
    }
}

static void copy_world(struct World *w) {
    size_t x;
    size_t y;

    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            struct Cell *c = cell(w, x, y);
            c->previous_state = c->current_state;
        }
    }
}

static bool equals_world(struct World *w) {
    size_t x;
    size_t y;

    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            struct Cell *c = cell(w, x, y);
            if (c->current_state != c->previous_state)
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
            if (0 != w->output && w->output_cells)
                print_world(w);
            w->state = CALCULATE;
            break;
        }
        case CALCULATE: {
            regenerate_world(w);
            if (0 != w->output && w->output_cells)
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

void clean_world(struct World *w, int value) {
    size_t x, y;

    for (x = 0; x < w->width; ++x) {
        for (y = 0; y < w->height; ++y) {
            struct Cell *c = cell(w, x, y);
            c->current_state = DEAD;
            c->previous_state = DEAD;
            c->x = x;
            c->y = y;
            c->value = value;
        }
    }
}

void toggle_live_world(struct World *w, size_t x, size_t y, int value) {
    struct Cell *c = cell(w, x, y);
    enum CellState state = c->current_state == ALIVE ? DEAD : ALIVE;
    c->previous_state = state;
    c->current_state = state;
    c->value = value;
}
