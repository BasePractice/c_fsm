#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <../game_life.h>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>
#include <assert.h>

#define WINDOW_WIDTH 1024
#define WINDOW_HEIGHT 768
#define QUAD_SIZE   20
#define PADDING_X   5
#define PADDING_Y   0
#define CONTROL_HEIGHT 20
#define WORLD_HEIGHT_MAX 135
#define WORLD_WIDTH_MAX 203

#define CELL_COLOR  BLACK
#define CELL_BORDER DARKGRAY

#define G_X   0
#define G_Y   0
#define G_Y1  4
#define G_Y2  8
#define G_Y3  12
#define G_Y4  16

static struct Cell g_Trimino[] = {
        {0, 0},
        {1, 0},
        {0, 1},
};

static struct Cell g_Glider[] = {
        {G_X, G_Y + 1},
        {G_X + 1, G_Y + 2},
        {G_X + 2, G_Y},
        {G_X + 2, G_Y + 1},
        {G_X + 2, G_Y + 2},
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

static Color colors[] = {
        BLACK, RED
};
#define COLOR_DEFAULT 0
#define COLOR_MARK    1

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
                                                                   sizeof(g_Cat[0]), .coord = g_Cat},
        {.name = "Trimino", .enable = false, .width = 3, .coord_size = sizeof(g_Trimino) /
                                                                       sizeof(g_Trimino[0]), .coord = g_Trimino}
};

enum State {
    Initialize, Running, PreSet
};

void update_world(struct World *world, int size) {
    size_t i, k, x_offset = size;

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
            size_t x = i;
            size_t y = k;
            struct Cell *c = cell(world, i, k);
            switch (c->current_state) {
                case ALIVE: {
                    if (world->output_cells) {
                        fprintf(world->output, "{%d, %d}, ", x, y);
                    }
                    struct Color color = colors[c->value];
                    DrawRectangle(x_offset + PADDING_X + (x * size), y * size, size, size, Fade(color, 0.9f));
                    DrawRectangleLines(x_offset + PADDING_X + (x * size), y * size, size, size,
                                       Fade(CELL_BORDER, 0.4f));
                    break;
                }
                default:
                case DEAD: {
                    DrawRectangle(x_offset + PADDING_X + (x * size), y * size, size, size, Fade(RAYWHITE, 0.6f));
                    DrawRectangleLines(x_offset + PADDING_X + (x * size), y * size, size, size,
                                       Fade(CELL_BORDER, 0.4f));
                    break;
                }
            }
        }
    }
    if (world->output_cells) {
        fprintf(world->output, "\n");
    }
}

int main(void) {
    int i, control_x, control_y, speed = -10, steps = 0, tick_gen = 0;
    int w_height = WORLD_HEIGHT_MAX, w_width = WORLD_WIDTH_MAX;
    int win_width = WINDOW_WIDTH, win_height = WINDOW_HEIGHT, quad_size = 50, quad_changed_size = 50;
    int i_figure = -1;
    bool running = false, next = false, drawing = false, clear = false, mark = false;
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
        running = GuiToggle((Rectangle) {control_x, control_y, 45, CONTROL_HEIGHT}, "Running", running);
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
        control_x += 60 + PADDING_X;
        control_y += PADDING_Y;
        w.output_cells = GuiButton((Rectangle) {control_x, control_y, 50, CONTROL_HEIGHT}, "Output");
        control_y += CONTROL_HEIGHT + PADDING_Y;
        clear = GuiButton((Rectangle) {control_x, control_y, 50, CONTROL_HEIGHT}, "Clear");
        control_y = 690 + PADDING_Y;
        control_x += 60 + PADDING_X;
        drawing = GuiToggle((Rectangle) {control_x, control_y, 50, CONTROL_HEIGHT}, "Drawing", drawing);
        control_y += CONTROL_HEIGHT + PADDING_Y;
        mark = GuiToggle((Rectangle) {control_x, control_y, 50, CONTROL_HEIGHT}, "Mark", mark);
        control_y += CONTROL_HEIGHT + PADDING_Y;


        if (drawing) {
            int x = GetMouseX();
            int y = GetMouseY();
            bool pressed = IsMouseButtonReleased(MOUSE_LEFT_BUTTON);
            if (pressed) {
                int i_x = (x - quad_size - PADDING_X) / quad_size;
                int i_y = (y - PADDING_Y) / (quad_size);
                if (i_x >= 0 && i_x < w.width && i_y >= 0 && i_y < w.height) {
//                    fprintf(w.output, "BEFORE\n");
//                    print_world(&w);
                    toggle_live_world(&w, i_x, i_y, mark ? COLOR_MARK : COLOR_DEFAULT);
//                    fprintf(w.output, "AFTER\n");
//                    print_world(&w);
//                    fprintf(stdout, "Pressed. x: %d((%d - %d) / (%d + %d) ), y: %d((%d) / (%d + %d) ), {%d, %d}. State: %s\n",
//                            x, x, quad_size, quad_size, PADDING_X,
//                            y, y, quad_size, PADDING_Y,
//                            i_x, i_y, cell->current_state == ALIVE ? "alive" : "dead");
                } else {
                    fprintf(stdout, "Illegal. {%d, %d}\n", i_x, i_y);
                };
            }
        } else if (clear) {
            clean_world(&w, COLOR_DEFAULT);
        } else {
            switch (state) {
                case PreSet: {
                    w_width = ((win_width - 10 - quad_size) / quad_size) - 1;
                    w_height = ((win_height - 100 - quad_size) / quad_size) - 1;
                    init_world(&w, w_width, w_height, 0, COLOR_DEFAULT);
                    preset_world(&w, figures[i_figure].width, figures[i_figure].coord, figures[i_figure].coord_size, COLOR_DEFAULT);
                    poll_world(&w);
                    state = Running;
                    steps = 0;
                    i_figure = -1;
                    break;
                }
                case Initialize: {
                    w_width = ((win_width - 10) / quad_size) - 1;
                    w_height = ((win_height - 100) / quad_size) - 1;
                    init_world(&w, w_width, w_height, 0, COLOR_DEFAULT);
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
        }
        update_world(&w, quad_size);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}