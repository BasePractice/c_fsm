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

enum State {
    Initialize, Running
};

void update_world(struct World *world, int size) {
    int i, k;

    for (i = 0; i < world->width; ++i) {
        for (k = 0; k < world->height; ++k) {
            switch (world->cells[k][i].current_state) {
                case ALIVE: {
                    DrawRectangle(PADDING_X + (i * size), k * size, size, size, Fade(RED, 0.8f));
                    break;
                }
                default:
                case DEAD: {
                    DrawRectangle(PADDING_X + (i * size), k * size, size, size, Fade(RAYWHITE, 0.6f));
                    break;
                }
            }
        }
    }
}

int main(void) {
    int i, control_x, control_y, speed = -5, steps = 0, tick_gen = 0;
    int w_height = WORLD_HEIGHT_MAX, w_width = WORLD_WIDTH_MAX;
    int win_width = WINDOW_WIDTH, win_height = WINDOW_HEIGHT, quad_size = QUAD_SIZE, quad_changed_size = QUAD_SIZE;
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
        speed = GuiSliderBar((Rectangle) {control_x + 40, control_y, 120, CONTROL_HEIGHT}, "Speed", NULL, speed, -10, 0);
        next = GuiButton((Rectangle) {control_x + 170, control_y, 50, CONTROL_HEIGHT}, "Generate");
        GuiSpinner((Rectangle) {control_x + 250, control_y, 60, CONTROL_HEIGHT}, "Size", &quad_changed_size, 1, 50, false);
        control_y += CONTROL_HEIGHT;
        running = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "Running", running);
        control_y += CONTROL_HEIGHT;
        DrawText(TextFormat("STEP: %d", steps), control_x, control_y, 10,
                 (steps >= 2000) ? MAROON : DARKGRAY);

        switch (state) {
            case Initialize: {
                w_width = (win_width - 10) / quad_size;
                w_height = (win_height - 100) / quad_size;
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