// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_SEQUENCE_H
#define C_FSM_SEQUENCE_H

#include <stdbool.h>
#include <stdint.h>

#define DO_SIZE 4

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
    int links;
    int does[4];
    int mutates;
    int enters;
    bool mutate_enable;
};

void sequence_create(struct Sequence *sequence, const char *text);

void sequence_destroy(struct Sequence *sequence);

enum SequenceFormat {
    FormatLinks, FormatEnters
};

void sequence_format(struct Sequence *sequence, enum SequenceFormat format);

char *node_to_string(struct Node *node);

char *sequence_to_string(struct Sequence *sequence);

void sequence_clone(struct Sequence *target, const struct Sequence *source);

void sequence_uml_fd(FILE *fd, struct Sequence *sequence);

void sequence_reset_enters(struct Sequence *sequence);

void sequence_reset_mutates(struct Sequence *sequence);

enum MutateStrategy {
    MutateEatDo = 0x0000000000000001,
    MutateEatNext = 0x0000000000000002,
    MutateNotDo = 0x0000000000000004,
    MutateNotNext = 0x0000000000000008,
    MutateAppend = 0x0000000000000010,
    MutateEvenDistribution = 0x0000000000000020,
    MutateEvenNodeEnters = 0x0000000000000040,
    MutateNotEnteredNode = 0x0000000000000080,
    MutateAllElements = 0x0000000000000100,
};

void sequence_mutate(struct Sequence *sequence, int strategy);

#endif //C_FSM_SEQUENCE_H
