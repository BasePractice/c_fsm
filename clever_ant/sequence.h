#ifndef C_FSM_SEQUENCE_H
#define C_FSM_SEQUENCE_H

#include <stdint.h>

enum SequenceStep {
    StepEatDo = 0,
    StepEatNext,
    StepNotDo,
    StepNotNext
};

struct Node;
struct Sequence {
    struct Node *node;
    size_t node_size;
};

struct Node {
    int id;
    int does[4];
};

void sequence_create(struct Sequence *sequence, const char *text);

void sequence_destroy(struct Sequence *sequence);

void sequence_format(struct Sequence *sequence);

char *sequence_to_string(struct Sequence *sequence);

#endif //C_FSM_SEQUENCE_H
