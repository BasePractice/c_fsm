#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <raylib.h>
#include <ant.h>
#include <sequence.h>
#include <clever_ant_gen.h>

#define RAYGUI_IMPLEMENTATION

#include <raygui.h>

#define QUAD_SIZE 20
#define PADDING_X   5
#define PADDING_Y   0
#define CONTROL_HEIGHT 20
#define SEQ "3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.9.2.19:3.8.0.11:3.7.1.17:3.10.3.2:3.8.3.4:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12:3.18.0.2:3.9.1.17"


enum Engine {
    Sequence_Prepare,
    Sequence_Step,
    Sequence_Check,
    Sequence_Mutate,
};


#define COND ctx->steps < best_steps
//#define COND ctx->steps < 2000

static void sequence_copy_mutate(struct Sequence *dst, struct Sequence *src) {
    size_t d;

    for (d = 0; d < dst->node_size && d < src->node_size; ++d) {
        dst->node[d].mutate_enable = src->node[d].mutate_enable;
    }
}

static enum CircleStep next_cb(struct AntContext *ctx, void *userdata) {
    static uint64_t best_steps = 2000;
    size_t i;

    if (COND) {
        best_steps = ctx->steps;
        sequence_format(ctx->sequence, FormatLinks);
        char *text = sequence_to_string(ctx->sequence);
        fprintf(stdout, "[%04llu] [%010llu gen] %s\n", best_steps, ctx->generation, text);
        for (i = 0; i < ctx->sequence->node_size; ++i) {
            fprintf(stdout, "[%04zu]:[%05d, %010d]\n", i,
                    ctx->sequence->node[i].enters, ctx->sequence->node[i].mutates);
        }
        fflush(stdout);
        free(text);
        return Best;
    }

    if (ctx->generation > 1000000000)
        return Complete;
    return Next;
}

void update_ctx(struct AntContext *ctx) {
    int i, k;

    for (i = 0; i < SQUARE_SIZE; ++i) {
        for (k = 0; k < SQUARE_SIZE; ++k) {
            switch (ctx->torus[i][k]) {
                case SQUARE_PATH: {
                    DrawRectangle(i * QUAD_SIZE, k * QUAD_SIZE, 20, 20, Fade(GREEN, 1.f));
                    break;
                }
                case SQUARE_APPLE: {
                    DrawRectangle(i * QUAD_SIZE, k * QUAD_SIZE, 20, 20, Fade(RED, 1.f));
                    break;
                }
                default:
                case SQUARE_EMPTY: {
                    DrawRectangle(i * QUAD_SIZE, k * QUAD_SIZE, 20, 20, Fade(LIGHTGRAY, 0.3f));
                    break;
                }
            }
        }
    }
    DrawRectangle(ctx->ant_x * QUAD_SIZE, ctx->ant_y * QUAD_SIZE, 20, 20, Fade(BLACK, 1.f));
}

int main(void) {
    enum Engine sequence_state = Sequence_Prepare;
    int i = 0, control_x = 0, control_y = 0, tick_step = 0;
    int speed = -90, steps = 2000;
    bool mutation_even_distrib = false, mutation_not_enter = false, mutation_even_enter = false,
            running = false, next = false, mutation_add = false, mutation_all = false;
    struct AntContext ctx = {0};
    struct Sequence sequence = {0, 0};
    struct Sequence origin = {0, 0};
    struct Sequence best_sequence = {0, 0};
    size_t c = 0;
    enum CircleStep step = Next;
    int strategy_origin = MutateEatNext | MutateNotNext | MutateNotDo, strategy = strategy_origin;

    srand(time(0));
    sequence_create(&origin, SEQ);
    sequence_clone(&best_sequence, &origin);
    sequence_clone(&sequence, &origin);
    InitWindow(800, 700, "Clever Ant");
    SetTargetFPS(60);
    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(RAYWHITE);
        for (i = 0; i < 31; ++i) {
            DrawLine(0, i * QUAD_SIZE, 31 * QUAD_SIZE, i * QUAD_SIZE, Fade(LIGHTGRAY, 0.6f));
        }
        DrawLine(0, 32 * QUAD_SIZE, GetScreenWidth(), 32 * QUAD_SIZE, Fade(LIGHTGRAY, 0.6f));

        for (i = 0; i < 32; ++i) {
            DrawLine(i * QUAD_SIZE, 0, i * QUAD_SIZE, 32 * QUAD_SIZE, Fade(LIGHTGRAY, 0.6f));
        }

        //Left pane

        control_x = 660 + PADDING_X;
        control_y = 20 + PADDING_Y;
        speed = GuiSliderBar((Rectangle) {control_x, control_y, 120, CONTROL_HEIGHT}, "Speed", NULL, speed, -100, 0);
        control_y += CONTROL_HEIGHT;
        steps = GuiSliderBar((Rectangle) {control_x, control_y, 120, CONTROL_HEIGHT}, "Steps", NULL, steps, 100, 2000);

        control_y += CONTROL_HEIGHT;
        control_y += CONTROL_HEIGHT;
        mutation_even_distrib = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "MutDistrib",
                                            mutation_even_distrib);
        control_y += CONTROL_HEIGHT;
        mutation_even_enter = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "MutEnters",
                                          mutation_even_enter);
        control_y += CONTROL_HEIGHT;
        mutation_not_enter = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "MutNotEnters",
                                         mutation_not_enter);
        control_y += CONTROL_HEIGHT;
        mutation_add = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "MutAdd",
                                   mutation_add);
        control_y += CONTROL_HEIGHT;
        mutation_all = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "MutAll",
                                   mutation_all);
        control_y += CONTROL_HEIGHT;
        running = GuiCheckBox((Rectangle) {control_x, control_y, 20, CONTROL_HEIGHT}, "Running", running);

        control_y += CONTROL_HEIGHT;
        control_y += CONTROL_HEIGHT;
        DrawText(TextFormat("STEP: %d", ctx.steps), control_x, control_y, 10,
                 (ctx.steps >= steps / 2) ? MAROON : DARKGRAY);
        control_y += CONTROL_HEIGHT;
        DrawText(TextFormat("APPLES: %d", ctx.apples), control_x, control_y, 10,
                 (ctx.apples >= APPLES / 2) ? MAROON : DARKGRAY);
        control_y += CONTROL_HEIGHT;
        DrawText(TextFormat("GEN: %d", ctx.generation), control_x, control_y, 10,
                 (ctx.generation >= 100) ? MAROON : DARKGRAY);

        control_y += CONTROL_HEIGHT;
        control_y += CONTROL_HEIGHT;
        next = GuiButton((Rectangle) {control_x, control_y, 40, CONTROL_HEIGHT}, "Next");

        //Bottom pane
        control_x = 5 + PADDING_X;
        control_y = (SQUARE_SIZE * QUAD_SIZE) + 20;
        if (0 != ctx.sequence) {
            for (i = 0; i < ctx.sequence->node_size; ++i) {
                ctx.sequence->node[i].mutate_enable
                        = GuiToggle((Rectangle) {control_x + (i * 35) + 5, control_y, 35, CONTROL_HEIGHT},
                                    node_to_string(&ctx.sequence->node[i]), ctx.sequence->node[i].mutate_enable);
            }
        }
        if (next) {
            sequence_state = Sequence_Mutate;
        }

        //Sequence
        switch (sequence_state) {
            default:
            case Sequence_Prepare: {
                context_next(&ctx, &sequence, APPLES, TORUS);
                sequence_reset_enters(ctx.sequence);
                ctx.torus_size = SQUARE_SIZE;
                sequence_state = Sequence_Step;
                tick_step = 0;
                break;
            }
            case Sequence_Step: {
                if (false == running) {
                    break;
                }
                if (tick_step - speed < 0) {
                    tick_step++;
                    break;
                }
                tick_step = 0;
                if (ctx.apples <= 0 || ctx.steps > steps) {
                    sequence_state = Sequence_Check;
                } else {
                    context_step(&ctx);
                }
                break;
            }
            case Sequence_Check: {
                step = next_cb(&ctx, 0);
                switch (step) {
                    default:
                    case Complete: {
                        continue;
                    }
                    case Best: {
                        sequence_clone(&best_sequence, &sequence);
                        c = 0;
                    }
                    case Next: {
                        strategy = strategy_origin;
                        if (mutation_even_distrib) {
                            strategy |= MutateEvenDistribution;
                        }
                        if (mutation_even_enter) {
                            strategy |= MutateEvenNodeEnters;
                        }
                        if (mutation_not_enter) {
                            strategy |= MutateNotEnteredNode;
                        }
                        if (mutation_all) {
                            strategy |= MutateAllElements;
                        }
                        ++c;
                        break;
                    }
                }
                sequence_state = Sequence_Mutate;
                break;
            }
            case Sequence_Mutate: {
                struct Sequence temp = {0};

                sequence_clone(&temp, &sequence);
                sequence_clone(&sequence, &best_sequence);
                sequence_copy_mutate(&sequence, &temp);
                sequence_destroy(&temp);
                if (c > sequence.node_size * 50000 || mutation_add) {
                    sequence_mutate(&sequence, MutateAppend);
                    sequence_reset_mutates(&sequence);
                    sequence_format(&sequence, FormatLinks);
                    c = 0;
                }
                sequence_mutate(&sequence, strategy);
                sequence_state = Sequence_Prepare;
                break;
            }
        }
        //
        update_ctx(&ctx);
        EndDrawing();
    }
    CloseWindow();
    return 0;
}