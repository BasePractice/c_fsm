#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "sequence.h"

static struct Node *node_add(struct Node *node, size_t node_size, int id, int does[4]) {
    if (0 == node) {
        node = malloc(sizeof(struct Node));
    } else {
        node = realloc(node, (node_size + 1) * sizeof(struct Node));
    }
    node[node_size].id = id;
    memcpy(&node[node_size].does, does, 4 * sizeof(int));
    return node;
}

static char *string_add(char *text, size_t *text_size, const char *append) {
    size_t append_size = strlen(append);
    if (0 == text) {
        text = malloc(append_size + 1);
        text[0] = 0;
    } else {
        text = realloc(text, append_size + (*text_size) + 1);
    }
    (*text_size) += append_size;
    strcat(text, append);
    text[(*text_size)] = 0;
    return text;
}

void sequence_create(struct Sequence *sequence, const char *text) {
    char value[256];
    int does[4];
    size_t text_size = strlen(text), i, k = 0, d = 0, c = 0;

    for (i = 0; i < text_size; ++i) {
        if ('.' == text[i] || ':' == text[i]) {
            value[k] = 0;
            does[d++] = strtol(value, 0, 10);
            if (':' == text[i]) {
                sequence->node = node_add(sequence->node, sequence->node_size++, c++, does);
                memset(does, 0, sizeof(int) * 4);
                d = 0;
            }
            k = 0;
            memset(value, 0, k);
            continue;
        }
        value[k++] = text[i];
    }

    if (i > 0) {
        value[i] = 0;
        does[d] = strtol(value, 0, 10);
        sequence->node = node_add(sequence->node, sequence->node_size++, c, does);
    }
}

void sequence_destroy(struct Sequence *sequence) {
    if (0 != sequence->node)
        free(sequence->node);
    sequence->node = 0;
    sequence->node_size = 0;
}

void sequence_format(struct Sequence *sequence) {
    size_t i;
    if (0 == sequence->node)
        return;
    for (i = 0; i < sequence->node_size; ++i) {

    }
}

char *sequence_to_string(struct Sequence *sequence) {
    size_t i, text_size = 0;
    char *text = 0, buf[20];
    if (0 != sequence->node) {
        for (i = 0; i < sequence->node_size; ++i) {
            if (i > 0) {
                text = string_add(text, &text_size, ":");
            }
            snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
                     sequence->node[i].does[StepEatDo], sequence->node[i].does[StepEatNext],
                     sequence->node[i].does[StepNotDo], sequence->node[i].does[StepNotNext]);
            text = string_add(text, &text_size, buf);
        }
    }
    return text;
}
