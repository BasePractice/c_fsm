#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>
#include "ant.h"
#include "clever_ant_gen.h"

static uint64_t generation = 0;

/**
 * 1. Для предоставления возможности генерировать(изменять) последовательность действий в состоянии, представим
 * состояние 4 числами, размера int. Где: первое число - это действие (последовательность действий) которое следует
 * выполнить при наличии на пути муравья еды; второе число - это состояние на которое следует перейти после исполнения
 * действий при наличии еды; треть число - это действие (последовательность) действий которое следует выполнить если
 * нет впереди муравья еды; четвертое число - это состояние в которое следует перейти если еды нет на пути муравья
 *
 * 2. Действия: 0 - поворот направо, 1 - поворо налево, 2 - шаг вперед, 3 - шаг вперед и съедание яблока
 */
#pragma push(pack)
#pragma pack(1)
struct StateSerialize {
    int eat_do;
    int eat_next;
    int emp_do;
    int emp_next;
};
#pragma pop(pack)


struct AntContext {
    uint64_t generation;

    int8_t ant_x;
    int8_t ant_y;
    uint8_t ant_state;
    uint8_t ant_direct;
    uint64_t steps;
    uint16_t apples;
    uint8_t torus[SQUARE_SIZE][SQUARE_SIZE];
    uint8_t torus_size;

    char *fsm_sequence;
    bool  output;
};

static uint32_t *sequence_add(uint32_t *sequence, size_t sequence_size, uint32_t value) {
    if (0 == sequence) {
        sequence = malloc(sizeof(uint32_t));
        sequence[0] = value;
    } else {
        sequence = realloc(sequence, (sequence_size + 1) * sizeof(uint32_t));
        sequence[sequence_size] = value;
    }
    return sequence;
}

static struct StateSerialize *sequence_parse(const char *sequence, size_t *len) {
    uint32_t *result = 0;
    size_t    result_size = 0;
    char value[256];
    size_t size = strlen(sequence), i, k = 0;

    (*len) = 0;
    for (i = 0; i < size; ++i) {
        if ('.' == sequence[i] || ':' == sequence[i]) {
            value[k] = 0;
            long v = strtol(value, 0, 10);
            result = sequence_add(result, result_size, v);
            result_size++;
            memset(value, 0, k);
            k = 0;
            if (':' == sequence[i]) {
                (*len)++;
            }
            continue;
        }
        value[k++] = sequence[i];
    }

    if (i > 0) {
        value[i] = 0;
        result = sequence_add(result, result_size, strtol(value, 0, 10));
        (*len)++;
    }
    return (struct StateSerialize *)result;
}

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

static void context_next(struct AntContext *ctx, uint64_t apples) {
    ctx->generation = generation++;
    ctx->ant_x = 0;
    ctx->ant_y = 0;
    ctx->ant_state = 0;
    ctx->ant_direct = DIRECT_RIGHT;
    ctx->steps = 0;
    ctx->apples = apples;
    ctx->fsm_sequence = 0;
}

char *context_get_do_name(int exec) {
    switch (exec) {
        case 0: {
            return "вправо";
        }
        case 1: {
            return "влево";
        }
        case 2: {
            return "вперед";
        }
        case 3: {
            return "вперед и поедание";
        }
        default: {
            return "неизвестное";
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

static inline void context_destroy(struct AntContext *ctx) {
    if (0 != ctx && 0 != ctx->fsm_sequence) {
        free(ctx->fsm_sequence);
    }
}

static uint64_t
context_run(struct AntContext *ctx, const char *sequence, const uint64_t complete_steps,
            const uint8_t torus[SQUARE_SIZE][SQUARE_SIZE]) {
    uint64_t i, k;
    struct StateSerialize *seq;
    size_t seq_size = 0;

    seq = sequence_parse(sequence, &seq_size);
    ctx->fsm_sequence = strdup(sequence);
    ctx->torus_size = SQUARE_SIZE;
    for (i = 0; i < SQUARE_SIZE; ++i) {
        for (k = 0; k < SQUARE_SIZE; ++k) {
            ctx->torus[i][k] = torus[i][k];
        }
    }
    if (ctx->output) {
        fprintf(stdout, "[%05llu] Последовательность: %s\n", ctx->generation, ctx->fsm_sequence);
        fflush(stdout);
    }
    while (ctx->apples > 0 && ctx->steps < complete_steps) {
        struct StateSerialize *s = &seq[ctx->ant_state];
        if (has_food(ctx)) {
            context_exec(ctx, s->eat_do);
            ctx->ant_state = s->eat_next;
        } else {
            context_exec(ctx, s->emp_do);
            ctx->ant_state = s->emp_next;
        }
        ++ctx->steps;
    }
    free(seq);
    if (ctx->output) {
        fprintf(stdout, "[%05llu] Всего шагов       : %llu\n", ctx->generation, ctx->steps);
        fflush(stdout);
    }
    return ctx->steps;
}

uint64_t
context_next_run(const char *sequence) {
    uint64_t result;
    struct AntContext ctx = {0};

    ctx.output = false;
    context_next(&ctx, APPLES);
    result = context_run(&ctx, sequence, 2000, TORUS);
    context_destroy(&ctx);
    return result;
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