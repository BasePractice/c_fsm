#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <../game_life.h>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define QUAD_SIZE   20
#define PADDING_X   5
#define PADDING_Y   0
#define CONTROL_HEIGHT 20
#define WORLD_HEIGHT_MAX 135
#define WORLD_WIDTH_MAX 203

static struct Cell g_Glider[] = {
        {0, 1},
        {1, 2},
        {2, 0},
        {2, 1},
        {2, 2}
};

static struct Cell g_Cat[] = {
        {1, 0},
        {4, 0},
        {1, 1},
        {2, 1},
        {3, 1},
        {4, 1},
        {0, 2},
        {5, 2},
        {0, 3},
        {2, 3},
        {3, 3},
        {5, 3},
        {0, 4},
        {5, 4},
        {1, 5},
        {2, 5},
        {3, 5},
        {4, 5}
};

struct Figure {
    const char *name;
    bool enable;
    size_t width;
    size_t coord_size;
    struct Cell *coord;
} figures[] = {
        {.name = "Glider", .enable = false, .width = 3, .coord_size = sizeof(g_Glider) /
                                                                      sizeof(g_Glider[0]), .coord = g_Glider},
        {.name = "Cat", .enable = false, .width = 6, .coord_size = sizeof(g_Cat) /
                                                                   sizeof(g_Cat[0]), .coord = g_Cat}
};

enum State {
    Initialize, Running, PreSet
};

void update_world(struct World *world, int size) {
    int i, k, x_offset = size, y_offset = size;

    for (k = 0; k < world->height; ++k) {
        GuiDrawText(TextFormat("%d", k),
                    (Rectangle) {0, (k * size), size, size},
                    GUI_TEXT_ALIGN_CENTER, DARKGRAY);
    }
    for (k = 0; k < world->width; ++k) {
        GuiDrawText(TextFormat("%d", k),
                    (Rectangle) {(k * size), world->height * size, size, size},
                    GUI_TEXT_ALIGN_CENTER, DARKGRAY);
    }

    for (i = 0; i < world->width; ++i) {
        for (k = 0; k < world->height; ++k) {
            switch (world->cells[i][k].current_state) {
                case ALIVE: {
                    DrawRectangle(x_offset + PADDING_X + (i * size), k * size, size, size, Fade(RED, 0.8f));
                    break;
                }
                default:
                case DEAD: {
                    DrawRectangle(x_offset + PADDING_X + (i * size), k * size, size, size, Fade(RAYWHITE, 0.6f));
                    break;
                }
            }
        }
    }
}

int main(void) {
    int i, control_x, control_y, speed = -10, steps = 0, tick_gen = 0;
    int w_height = WORLD_HEIGHT_MAX, w_width = WORLD_WIDTH_MAX;
    int win_width = WINDOW_WIDTH, win_height = WINDOW_HEIGHT, quad_size = QUAD_SIZE, quad_changed_size = QUAD_SIZE;
    int i_figure = -1;
    bool running = false, next = false;
    struct World w = {0};
    enum State state = Initialize;

    srand(time(0));
    InitWindow(win_width, win_height, "Game life");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);


        control_x = PADDING_X;
        control_y = 690 + PADDING_Y;
        speed = GuiSliderBar((Rectangle) {control_x + 40, control_y, 120, CONTROL_HEIGHT}, "Speed", NULL, speed, -20,
                             0);
        next = GuiButton((Rectangle) {control_x + 170, control_y, 50, CONTROL_HEIGHT}, "Generate");
        GuiSpinner((Rectangle) {control_x + 250, control_y, 60, CONTROL_HEIGHT}, "Size", &quad_changed_size, 1, 50,
                   false);
        control_y += CONTROL_HEIGHT;
        running = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "Running", running);
        control_y += CONTROL_HEIGHT;
        DrawText(TextFormat("STEP: %d", steps), control_x, control_y, 10,
                 (steps >= 2000) ? MAROON : DARKGRAY);
        control_x = PADDING_X + 400;
        control_y = 690 + PADDING_Y;

        for (i = 0; i < sizeof(figures) / sizeof(figures[0]); ++i) {
            figures[i].enable = GuiButton((Rectangle) {control_x, control_y + (i * CONTROL_HEIGHT), 50, CONTROL_HEIGHT},
                                          figures[i].name);
            if (true == figures[i].enable) {
                i_figure = i;
            }
        }

        switch (state) {
            case PreSet: {
                w_width = ((win_width - 10 - quad_size) / quad_size);
                w_height = ((win_height - 100 - quad_size) / quad_size);
                init_world(&w, w_width, w_height, 0);
                preset_world(&w, figures[i_figure].width, figures[i_figure].coord, figures[i_figure].coord_size);
                poll_world(&w);
                state = Running;
                steps = 0;
                i_figure = -1;
                break;
            }
            case Initialize: {
                w_width = ((win_width - 10) / quad_size);
                w_height = ((win_height - 100) / quad_size);
                init_world(&w, w_width, w_height, 0);
                poll_world(&w);
                state = Running;
                steps = 0;
                break;
            }
            default:
            case Running: {
                if (quad_changed_size != quad_size) {
                    quad_size = quad_changed_size;
                    state = Initialize;
                    break;
                }
                if (i_figure >= 0) {
                    state = PreSet;
                    break;
                }
                if (true == next) {
                    state = Initialize;
                    break;
                }
                if (false == running)
                    break;
                if (tick_gen + speed < 0) {
                    tick_gen++;
                } else {
                    tick_gen = 0;
                    poll_world(&w);
                    steps++;
                }
                break;
            }
        }
        update_world(&w, quad_size);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}