#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include "transport_loader.h"

#define MAZE_ROW 10
#define MAZE_COL 10

#define MAZE_EMPTY  0
#define MAZE_START  1
#define MAZE_WALL   2
#define MAZE_PATH   3
#define MAZE_USED   4
#define MAZE_STOP   5

static uint8_t DIRECTIONS[4][2] = { { 0, 1 }, { 1, 0 }, { 0, -1 }, { -1, 0 } };
static uint8_t g_Maze[MAZE_ROW][MAZE_COL] = {
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2},
        {2, 0, 2, 2, 0, 5, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 2, 2, 0, 0, 2, 2, 0, 2},
        {2, 0, 0, 0, 0, 0, 0, 0, 0, 2},
        {2, 0, 1, 0, 0, 0, 0, 0, 0, 2},
        {2, 2, 2, 2, 2, 2, 2, 2, 2, 2}
};

struct Coordinate {
    uint16_t x;
    uint16_t y;
} coordinates[(MAZE_ROW * MAZE_COL) / 2];

static struct Path {
    struct Coordinate *c;
    uint32_t len;
} path = {.c = coordinates, .len = (MAZE_ROW * MAZE_COL) / 2};
static int32_t path_it = 0;

static void print(uint8_t maze[MAZE_ROW][MAZE_COL]) {
    int r, c;

    for (r = 0; r < MAZE_ROW; ++r) {
        for (c = 0; c < MAZE_COL; ++c) {
            switch (maze[r][c]) {
                case MAZE_EMPTY: {
                    fprintf(stdout, " ");
                    break;
                }
                case MAZE_START: {
                    fprintf(stdout, "8");
                    break;
                }
                case MAZE_WALL: {
                    fprintf(stdout, "*");
                    break;
                }
                case MAZE_PATH: {
                    fprintf(stdout, "=");
                    break;
                }
                case MAZE_STOP: {
                    fprintf(stdout, "X");
                    break;
                }
                default:
                case MAZE_USED: {
                    fprintf(stdout, "U");
                    break;
                }
            }
        }
        fprintf(stdout, "\n");
    }
}

static void resolve(uint8_t maze[MAZE_ROW][MAZE_COL]) {
    struct Coordinate start = {.x = 8, .y = 2};
    bool running = true;
    path.c[path_it++] = start;
    do {
        --path_it;
        struct Coordinate c = path.c[path_it];
        if (c.x >= MAZE_ROW || c.y >= MAZE_COL)
            continue;
        uint8_t d = maze[c.x][c.y];
        switch (d) {
            case MAZE_STOP: {
                running = false;
                continue;
            }
            case MAZE_PATH:
            case MAZE_USED:
            case MAZE_WALL: {
                continue;
            }
            case MAZE_START:
            default: {
                uint8_t i;

                maze[c.x][c.y] = MAZE_USED;
                for (i = 0; i < 4; ++i) {
                    uint8_t x = c.x + DIRECTIONS[i][0];
                    uint8_t y = c.y + DIRECTIONS[i][1];

                    if ( x >= 0 && x < MAZE_ROW && y >= 0 && y < MAZE_COL) {
                        path.c[path_it].x = x;
                        path.c[path_it].y = y;
                        path_it++;
                    }
                }
                break;
            }
        }
        print(maze);
        fprintf(stdout, "\n");
    } while (path_it > 0 && running);
}


int
main(int argc, char **argv) {
    struct Configuration configuration;

    read_configuration(&configuration, "factory.json");
    print(g_Maze);
    resolve(g_Maze);
    return EXIT_SUCCESS;
}
