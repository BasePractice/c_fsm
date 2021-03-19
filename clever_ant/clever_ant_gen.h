#ifndef C_FSM_CLEVER_ANT_GEN_H
#define C_FSM_CLEVER_ANT_GEN_H

#include <stdbool.h>
#include <ant.h>

#if defined(__cplusplus)
extern "C" {
#endif
struct Sequence;

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

    struct Sequence *sequence;
};

void context_destroy(struct AntContext *ctx);

void context_next(struct AntContext *ctx, struct Sequence *sequence, uint64_t apples,
                  const uint8_t torus[SQUARE_SIZE][SQUARE_SIZE]);

uint64_t context_run(struct AntContext *ctx, uint64_t complete_steps);
void context_step(struct AntContext *ctx);


enum CircleStep {
    Next, Best, Complete
};

void context_circle_sequence(struct Sequence *sequence,
                             int strategy,
                             enum CircleStep (*next_cb)(struct AntContext *ctx, void *userdata),
                             void *userdata);

#if defined(__cplusplus)
}
#endif

#endif //C_FSM_CLEVER_ANT_GEN_H
