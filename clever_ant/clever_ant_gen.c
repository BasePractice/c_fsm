#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <assert.h>
#include "ant.h"
#include "sequence.h"
#include "clever_ant_gen.h"

/**
 * 1. Для предоставления возможности генерировать(изменять) последовательность действий в состоянии, представим
 * состояние 4 числами, размера int. Где: первое число - это действие (последовательность действий) которое следует
 * выполнить при наличии на пути муравья еды; второе число - это состояние на которое следует перейти после исполнения
 * действий при наличии еды; треть число - это действие (последовательность) действий которое следует выполнить если
 * нет впереди муравья еды; четвертое число - это состояние в которое следует перейти если еды нет на пути муравья
 *
 * 2. Действия: 0 - поворот направо, 1 - поворо налево, 2 - шаг вперед, 3 - шаг вперед и съедание яблока
 */

static inline void eat(struct AntContext *ctx) {
    if (ctx->torus[ctx->ant_x][ctx->ant_y] == SQUARE_APPLE) {
        --ctx->apples;
        ctx->torus[ctx->ant_x][ctx->ant_y] = SQUARE_PATH;
    }
}

static inline void step(struct AntContext *ctx) {
    if (ctx->ant_direct == DIRECT_RIGHT) {
        ++ctx->ant_y;
        if (ctx->ant_y >= ctx->torus_size)
            ctx->ant_y = 0;
    } else if (ctx->ant_direct == DIRECT_LEFT) {
        --ctx->ant_y;
        if (ctx->ant_y < 0)
            ctx->ant_y = ctx->torus_size - 1;
    } else if (ctx->ant_direct == DIRECT_UP) {
        --ctx->ant_x;
        if (ctx->ant_x < 0)
            ctx->ant_x = ctx->torus_size - 1;
    } else if (ctx->ant_direct == DIRECT_BOTTOM) {
        ++ctx->ant_x;
        if (ctx->ant_x >= ctx->torus_size)
            ctx->ant_x = 0;
    }
}

static inline void turn_left(struct AntContext *ctx) {
    --ctx->ant_direct;
    if (ctx->ant_direct < DIRECT_UP)
        ctx->ant_direct = DIRECT_LEFT;
}

static inline void turn_right(struct AntContext *ctx) {
    ++ctx->ant_direct;
    if (ctx->ant_direct > DIRECT_LEFT)
        ctx->ant_direct = DIRECT_UP;
}

static inline bool has_food(struct AntContext *ctx) {
    if (ctx->ant_direct == DIRECT_RIGHT) {
        if (ctx->ant_y + 1 >= ctx->torus_size)
            return ctx->torus[ctx->ant_x][0] == SQUARE_APPLE;
        return ctx->torus[ctx->ant_x][ctx->ant_y + 1] == SQUARE_APPLE;
    } else if (ctx->ant_direct == DIRECT_LEFT) {
        if (ctx->ant_y - 1 < 0)
            return ctx->torus[ctx->ant_x][ctx->torus_size - 1] == SQUARE_APPLE;
        return ctx->torus[ctx->ant_x][ctx->ant_y - 1] == SQUARE_APPLE;
    } else if (ctx->ant_direct == DIRECT_UP) {
        if (ctx->ant_x - 1 < 0)
            return ctx->torus[ctx->torus_size - 1][ctx->ant_y] == SQUARE_APPLE;
        return ctx->torus[ctx->ant_x - 1][ctx->ant_y] == SQUARE_APPLE;
    } else if (ctx->ant_direct == DIRECT_BOTTOM) {
        if (ctx->ant_x + 1 >= ctx->torus_size)
            return ctx->torus[0][ctx->ant_y] == SQUARE_APPLE;
        return ctx->torus[ctx->ant_x + 1][ctx->ant_y] == SQUARE_APPLE;
    }
    assert(false);
    return false;
}

void context_next(struct AntContext *ctx, struct Sequence *sequence, uint64_t apples,
                  const uint8_t torus[SQUARE_SIZE][SQUARE_SIZE]) {
    size_t i, k;

    ctx->generation++;
    ctx->ant_x = 0;
    ctx->ant_y = 0;
    ctx->ant_state = 0;
    ctx->ant_direct = DIRECT_RIGHT;
    ctx->steps = 0;
    ctx->apples = apples;
    ctx->sequence = sequence;

    for (i = 0; i < SQUARE_SIZE; ++i) {
        for (k = 0; k < SQUARE_SIZE; ++k) {
            ctx->torus[i][k] = torus[i][k];
        }
    }
}

static void context_exec(struct AntContext *ctx, int exec) {
    switch (exec) {
        case 0: {
            turn_right(ctx);
            break;
        }
        case 1: {
            turn_left(ctx);
            break;
        }
        case 2: {
            step(ctx);
            break;
        }
        case 3: {
            step(ctx);
            eat(ctx);
            break;
        }
        default: {
            assert(false);
            break;
        }
    }
}

void context_destroy(struct AntContext *ctx) {
    if (0 != ctx && 0 != ctx->sequence) {
        sequence_destroy(ctx->sequence);
    }
}

uint64_t
context_run(struct AntContext *ctx, const uint64_t complete_steps) {

    sequence_reset_enters(ctx->sequence);
    ctx->torus_size = SQUARE_SIZE;
    while (ctx->apples > 0 && ctx->steps < complete_steps) {
        struct Node *n = &ctx->sequence->node[ctx->ant_state];
        n->enters++;
        if (has_food(ctx)) {
            context_exec(ctx, n->does[StepEatDo]);
            ctx->ant_state = n->does[StepEatNext];
        } else {
            context_exec(ctx, n->does[StepNotDo]);
            ctx->ant_state = n->does[StepNotNext];
        }
        ++ctx->steps;
    }
    return ctx->steps;
}

void context_circle_sequence(struct Sequence *origin,
                             int origin_strategy,
                             enum CircleStep (*next_cb)(struct AntContext *ctx, void *userdata),
                             void *userdata) {
    struct AntContext ctx = {0};
    struct Sequence sequence = {0, 0};
    struct Sequence best_sequence = {0, 0};
    size_t c = 0;
    enum CircleStep step = Next;

    sequence_clone(&sequence, origin);
    sequence_clone(&best_sequence, origin);
    while (step != Complete) {
        int strategy = origin_strategy;

        context_next(&ctx, &sequence, APPLES, TORUS);
        context_run(&ctx, 2000);
        step = (*next_cb)(&ctx, userdata);
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
                if (0 == c % 666) {
                    strategy |= MutateEvenDistribution;
                } else if (0 == c % 333) {
                    strategy |= MutateEvenNodeEnters;
                } else if (0 == c % 777) {
                    strategy |= MutateNotEnteredNode;
                } else
                if (c > sequence.node_size * 50000) {
//                    sequence_destroy(&sequence);
//                    sequence_clone(&sequence, &best_sequence);
                    sequence_mutate(&sequence, MutateAppend);
                    sequence_reset_mutates(&sequence);
                    sequence_format(&sequence, FormatLinks);
                    c = 0;
                }
                sequence_mutate(&sequence, strategy);
                ++c;
                break;
            }
        }
    }
    context_destroy(&ctx);
}

/**
 * Представление автомата по умолчанию при помощи последовательности из 4 чисел
 * Действия: 0 - поворот направо, 1 - поворо налево, 2 - шаг вперед, 3 - шаг вперед и съедание яблока
 * 0. 3001
 * 1. 3002
 * 2. 3003
 * 3. 3004
 * 4. 3020
 *
 * Итого: 3001 3002 3003 3004 3020
 */

/*http://www.demo.cs.brandeis.edu/papers/ep93.pdf */
//int main() {
//    context_next_run("3.0.0.1:3.0.0.2:3.0.0.3:3.0.0.4:3.0.2.0");
//    return EXIT_SUCCESS;
//}