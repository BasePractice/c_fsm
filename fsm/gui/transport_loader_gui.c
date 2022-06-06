#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>


#define MAZE_ROW 10
#define MAZE_COL 10

#define MAZE_EMPTY  0
#define MAZE_START  1
#define MAZE_WALL   2
#define MAZE_PATH   3
#define MAZE_USED   4
#define MAZE_STOP   5

static uint8_t DIRECTIONS[4][2] = {{0,  1},
                                   {1,  0},
                                   {0,  -1},
                                   {-1, 0}};
static uint8_t g_Original[MAZE_ROW][MAZE_COL] = {
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};
static uint8_t g_Maze[MAZE_ROW][MAZE_COL] = {
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

struct Coordinate {
    uint16_t x;
    uint16_t y;
    struct Coordinate *up;
};

static struct Path {
    struct Coordinate **c;
    uint32_t len;
} path = {.c = 0, .len = 0};

static struct Coordinate start = {.x = 8, .y = 2};
static struct Coordinate stop = {.x = 2, .y = 8};

#define MAZE_QUAD_SIZE 20
#define GUI_MAZE_WIDTH   (MAZE_COL * MAZE_QUAD_SIZE)
#define GUI_MAZE_HEIGHT  (MAZE_ROW * MAZE_QUAD_SIZE)

static void print(uint8_t maze[MAZE_ROW][MAZE_COL]) {
    int x, y;

//    for (y = 0; y < MAZE_COL; ++y) {
//        DrawLine(0, y * MAZE_QUAD_SIZE, 32 * MAZE_QUAD_SIZE, y * MAZE_QUAD_SIZE, Fade(LIGHTGRAY, 0.6f));
//    }
//    for (x = 0; x < MAZE_ROW; ++x) {
//        DrawLine(x * MAZE_QUAD_SIZE, 0, x * MAZE_QUAD_SIZE, 32 * MAZE_QUAD_SIZE, Fade(LIGHTGRAY, 0.6f));
//    }

    for (x = 0; x < MAZE_ROW; ++x) {
        for (y = 0; y < MAZE_COL; ++y) {
            switch (maze[x][y]) {
                case MAZE_EMPTY: {
                    break;
                }
                case MAZE_START: {
                    DrawRectangle(y * MAZE_QUAD_SIZE, x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                                  Fade(GREEN, 0.3f));
                    break;
                }
                case MAZE_WALL: {
                    DrawRectangle(y * MAZE_QUAD_SIZE, x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                                  Fade(BROWN, 0.3f));
                    break;
                }
                case MAZE_PATH: {
                    DrawRectangle(y * MAZE_QUAD_SIZE, x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                                  Fade(ORANGE, 0.3f));
                    break;
                }
                case MAZE_STOP: {
                    DrawRectangle(y * MAZE_QUAD_SIZE, x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                                  Fade(RED, 0.3f));
                    break;
                }
                default:
                case MAZE_USED: {
                    DrawRectangle(y * MAZE_QUAD_SIZE, x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, Fade(GRAY, 0.3f));
                    break;
                }
            }
        }
    }
    DrawRectangle(start.y * MAZE_QUAD_SIZE, start.x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                  Fade(GREEN, 0.3f));
    DrawRectangle(stop.y * MAZE_QUAD_SIZE, stop.x * MAZE_QUAD_SIZE, MAZE_QUAD_SIZE, MAZE_QUAD_SIZE,
                  Fade(RED, 0.3f));
}

bool running = true;

static void path_clean() {
    uint8_t i, j;

    path.c = 0;
    path.len = 0;
    running = true;
    for (i = 0; i < MAZE_ROW; ++i) {
        for (j = 0; j < MAZE_COL; ++j) {
            g_Maze[i][j] = g_Original[i][j];
        }
    }
    g_Maze[stop.x][stop.y] = MAZE_STOP;
}

static struct Coordinate *path_add(struct Coordinate *up, uint16_t x, uint16_t y) {
    struct Coordinate *c, **p;

    ++path.len;
    c = calloc(1, sizeof(struct Coordinate));
    c->y = y;
    c->x = x;
    c->up = up;
    if (0 == path.c) {
        p = (struct Coordinate **) calloc(1, sizeof(struct Coordinate *));
    } else {
        p = (struct Coordinate **) realloc(path.c, path.len * sizeof(struct Coordinate *));
    }
    path.c = p;
    path.c[path.len - 1] = c;
    return path.c[path.len - 1];
}

static struct Coordinate *path_pop() {
    struct Coordinate *r = 0;
    if (0 != path.c && path.len > 0) {
        r = path.c[--path.len];
    }
    return r;
}

static struct Coordinate *path_peek() {
    struct Coordinate *r = 0;
    if (0 != path.c && path.len > 0) {
        r = path.c[path.len - 1];
    }
    return r;
}

static void resolve(uint8_t maze[MAZE_ROW][MAZE_COL], bool step) {
    if (!step)
        return;
    struct Coordinate *c = path_pop();
    if (c == NULL || c->x >= MAZE_ROW || c->y >= MAZE_COL)
        return;
    uint8_t d = maze[c->x][c->y];
    switch (d) {
        case MAZE_STOP: {
            struct Coordinate *it;
            for (it = c; it != 0; it = it->up) {
                maze[it->x][it->y] = MAZE_PATH;
            }
            running = false;
            return;
        }
        case MAZE_PATH:
        case MAZE_USED:
        case MAZE_WALL: {
            return;
        }
        case MAZE_START:
        default: {
            uint32_t i;

            maze[c->x][c->y] = MAZE_USED;
            for (i = 0; i < 4; ++i) {
                uint8_t x = c->x + DIRECTIONS[i][0];
                uint8_t y = c->y + DIRECTIONS[i][1];

                if (x >= 0 && x < MAZE_ROW && y >= 0 && y < MAZE_COL
                    && maze[x][y] != MAZE_PATH && maze[x][y] != MAZE_USED && maze[x][y] != MAZE_WALL) {
                    path_add(c, x, y);
                }
            }
            break;
        }
    }
}

int
main(int argc, char **argv) {
    float y = GUI_MAZE_HEIGHT + (MAZE_QUAD_SIZE), x = 5;
    bool step, reset = true;

    InitWindow(GUI_MAZE_WIDTH, GUI_MAZE_HEIGHT + 120, "Transport");
    SetTargetFPS(60);

    g_Maze[stop.x][stop.y] = MAZE_STOP;
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        if (reset) {
            path_clean();
            path_add(0, start.x, start.y);
            reset = false;
        }
        step = GuiButton((Rectangle) {x, y, 40, 20}, "Step");
        reset = GuiButton((Rectangle) {x + 40, y, 40, 20}, "Reset");
        resolve(g_Maze, step);
        print(g_Maze);
        EndDrawing();
    }
    CloseWindow();
    return EXIT_SUCCESS;
}
