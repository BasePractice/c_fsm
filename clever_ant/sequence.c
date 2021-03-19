#include <stdio.h>
#include <stdbool.h>
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
    node[node_size].links = 0;
    node[node_size].mutates = 0;
    node[node_size].enters = 0;
    node[node_size].mutate_enable = true;
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

static inline int node_find_index(struct Node *node, size_t node_size, int id) {
    size_t i;

    for (i = 0; i < node_size; ++i) {
        if (node[i].id == id)
            return i;
    }
    return -1;
}

void sequence_clone(struct Sequence *target, const struct Sequence *source) {
    sequence_destroy(target);
    target->node_size = source->node_size;
    target->node = malloc(source->node_size * sizeof(struct Node));
    memcpy(target->node, source->node, sizeof(struct Node) * source->node_size);
}

void sequence_format(struct Sequence *sequence, enum SequenceFormat format) {
    size_t i;
    struct Node *new_node = 0;
    size_t new_node_size = 0;

    if (0 == sequence->node)
        return;

    if (FormatEnters == format) {
        for (i = 0; i < sequence->node_size; ++i) {
            if (0 == sequence->node[i].enters) {
                continue;
            } else {
                new_node = node_add(new_node, new_node_size,
                                    sequence->node[i].id, sequence->node[i].does);
                new_node[new_node_size].mutates = sequence->node[i].mutates;
                new_node[new_node_size].enters = sequence->node[i].enters;
                new_node[new_node_size].mutate_enable = sequence->node[i].mutate_enable;
                new_node_size++;
            }
        }
    } else if (FormatLinks == format) {
        for (i = 0; i < sequence->node_size; ++i) {
            sequence->node[sequence->node[i].does[StepEatNext]].links++;
            sequence->node[sequence->node[i].does[StepNotNext]].links++;
        }

        for (i = 0; i < sequence->node_size; ++i) {
            if (0 == sequence->node[i].links) {
                continue;
            } else {
                new_node = node_add(new_node, new_node_size,
                                    sequence->node[i].id, sequence->node[i].does);
                new_node[new_node_size].mutates = sequence->node[i].mutates;
                new_node[new_node_size].enters = sequence->node[i].enters;
                new_node[new_node_size].mutate_enable = sequence->node[i].mutate_enable;
                new_node_size++;
            }
        }
    } else {
        for (i = 0; i < sequence->node_size; ++i) {
            new_node = node_add(new_node, new_node_size,
                                sequence->node[i].id, sequence->node[i].does);
            new_node[new_node_size].mutates = sequence->node[i].mutates;
            new_node[new_node_size].enters = sequence->node[i].enters;
            new_node[new_node_size].mutate_enable = sequence->node[i].mutate_enable;
            new_node_size++;
        }
    }

    for (i = 0; i < new_node_size; ++i) {
        int index_eat = node_find_index(new_node, new_node_size, new_node[i].does[StepEatNext]);
        int index_not = node_find_index(new_node, new_node_size, new_node[i].does[StepNotNext]);
        if (-1 == index_eat || -1 == index_not)
            continue;
        new_node[i].does[StepEatNext] = index_eat;
        new_node[i].does[StepNotNext] = index_not;
    }

    for (i = 0; i < new_node_size; ++i) {
        new_node[i].id = i;
    }

    sequence_destroy(sequence);
    sequence->node = new_node;
    sequence->node_size = new_node_size;
}

char *sequence_to_string(struct Sequence *sequence) {
    size_t i, text_size = 0;
    char *text = 0;
    if (0 != sequence->node) {
        for (i = 0; i < sequence->node_size; ++i) {
            if (i > 0) {
                text = string_add(text, &text_size, ":");
            }
            text = string_add(text, &text_size, node_to_string(&sequence->node[i]));
        }
    }
    return text;
}

char *node_to_string(struct Node *node) {
    static char buf[20];

    snprintf(buf, sizeof(buf), "%d.%d.%d.%d",
             node->does[StepEatDo], node->does[StepEatNext],
             node->does[StepNotDo], node->does[StepNotNext]);
    return buf;
}

static inline char *sequence_get_does_name(int _do) {
    switch (_do) {
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

void sequence_uml_fd(FILE *fd, struct Sequence *sequence) {
    const char *uml_start = "@startuml\n[*] --> S_0\n";
    const char *uml_end = "@enduml\n";
    char *uml = 0, buf[256];
    size_t uml_allocated = 0, i;

    uml = string_add(uml, &uml_allocated, uml_start);
    for (i = 0; i < sequence->node_size; ++i) {
        snprintf(buf, sizeof(buf), "S_%d --> S_%d: яблоко/%s\n",
                 sequence->node[i].id, sequence->node[sequence->node[i].does[StepEatNext]].id,
                 sequence_get_does_name(sequence->node[i].does[StepEatDo]));
        uml = string_add(uml, &uml_allocated, buf);
        snprintf(buf, sizeof(buf), "S_%d --> S_%d: пусто/%s\n",
                 sequence->node[i].id, sequence->node[sequence->node[i].does[StepNotNext]].id,
                 sequence_get_does_name(sequence->node[i].does[StepNotDo]));
        uml = string_add(uml, &uml_allocated, buf);
    }
    uml = string_add(uml, &uml_allocated, "\n");
    uml = string_add(uml, &uml_allocated, uml_end);
    fprintf(fd, "%s", uml);
}

struct SortedNode {
    int id;
    int mutates;
    int enters;
};

static void sequence_mutate_item(const struct Sequence *sequence, int strategy, int place);

static int sorted_node_compare_mutates(const void *n1, const void *n2) {
    if (((struct SortedNode *) n1)->mutates < ((struct SortedNode *) n2)->mutates) {
        return 1;
    } else if (((struct SortedNode *) n1)->mutates > ((struct SortedNode *) n2)->mutates) {
        return -1;
    }
    return 0;
}

static int sorted_node_compare_enters(const void *n1, const void *n2) {
    if (((struct SortedNode *) n1)->enters > ((struct SortedNode *) n2)->enters) {
        return 1;
    } else if (((struct SortedNode *) n1)->enters < ((struct SortedNode *) n2)->enters) {
        return -1;
    }
    return 0;
}

#define RANDOM(place) \
    do {              \
        (place) = rand() % sequence->node_size + 0; \
    } while ( false == sequence->node[(place)].mutate_enable );


void sequence_mutate(struct Sequence *sequence, int strategy) {
    int place = 0;

    RANDOM(place);
    if ((strategy & MutateAppend) == MutateAppend) {
        sequence->node = node_add(
                sequence->node, sequence->node_size, sequence->node_size,
                sequence->node[sequence->node_size - 1].does);
        sequence->node[sequence->node_size].does[StepEatNext] = 0;
        sequence->node[sequence->node_size].does[StepEatDo] = 3;
        sequence->node[sequence->node_size].does[StepNotNext] = 0;
        sequence->node[sequence->node_size].does[StepNotDo] = rand() % DO_SIZE - 0;
        sequence->node[sequence->node_size - 1].does[StepNotNext] = sequence->node_size;
        sequence->node_size++;
    }

    if ((strategy & MutateNotEnteredNode) == MutateNotEnteredNode) {
        sequence_format(sequence, FormatEnters);
        sequence_format(sequence, FormatLinks);
        RANDOM(place);
    }
    if ((strategy & MutateEvenDistribution) == MutateEvenDistribution ||
        (strategy & MutateEvenNodeEnters) == MutateEvenNodeEnters) {
        static struct SortedNode *sorted_node = 0;
        static size_t sorted_node_size = 0, i;
        size_t div;

        if (0 == sorted_node) {
            sorted_node_size = sequence->node_size;
            sorted_node = malloc(sorted_node_size * sizeof(struct SortedNode));
        } else if (sorted_node_size < sequence->node_size) {
            sorted_node_size = sequence->node_size;
            sorted_node = realloc(sorted_node, sorted_node_size * sizeof(struct SortedNode));
        }
        for (i = 0; i < sequence->node_size; ++i) {
            sorted_node[i].mutates = sequence->node[i].mutates;
            sorted_node[i].enters = sequence->node[i].enters;
            sorted_node[i].id = sequence->node[i].id;
        }

        if ((strategy & MutateEvenDistribution) == MutateEvenDistribution) {
            qsort(sorted_node, sorted_node_size, sizeof(struct SortedNode), sorted_node_compare_mutates);
        } else if ((strategy & MutateEvenNodeEnters) == MutateEvenNodeEnters) {
            qsort(sorted_node, sorted_node_size, sizeof(struct SortedNode), sorted_node_compare_enters);
        }
        div = sequence->node_size;
        if (div < 2)
            div = 2;
        place = sorted_node[rand() % (div / 2)].id; //Выбираем из первой половины
        if (false == sequence->node[(place)].mutate_enable) {
            RANDOM(place);
        }
    }

    if ((strategy & MutateAllElements) == MutateAllElements) {
        size_t i;

        for (i = 0; i < sequence->node_size; ++i) {
            if (false == sequence->node[i].mutate_enable)
                continue;
            sequence_mutate_item(sequence, strategy, i);
        }
    } else {
        sequence_mutate_item(sequence, strategy, place);
    }
}

static void sequence_mutate_item(const struct Sequence *sequence, int strategy, int place) {
    if ((strategy & MutateEatDo) == MutateEatDo) {
        sequence->node[place].does[StepEatDo] = rand() % DO_SIZE - 0;
        sequence->node[place].mutates++;
    }
    if ((strategy & MutateEatNext) == MutateEatNext) {
        do {
            sequence->node[place].does[StepEatNext] = rand() % sequence->node_size - 0;
        } while (place != sequence->node[place].does[StepEatNext] &&
                 sequence->node[place].does[StepEatNext] != sequence->node[place].does[StepNotNext]);
        sequence->node[place].mutates++;
    }

    if ((strategy & MutateNotDo) == MutateNotDo) {
        sequence->node[place].does[StepNotDo] = rand() % DO_SIZE - 0;
        sequence->node[place].mutates++;
    }
    if ((strategy & MutateNotNext) == MutateNotNext) {
        do {
            sequence->node[place].does[StepNotNext] = rand() % sequence->node_size - 0;
        } while (place != sequence->node[place].does[StepNotNext] &&
                 sequence->node[place].does[StepNotNext] != sequence->node[place].does[StepEatNext]);
        sequence->node[place].mutates++;
    }
}

void sequence_reset_enters(struct Sequence *sequence) {
    size_t i;

    for (i = 0; i < sequence->node_size; ++i) {
        sequence->node[i].enters = 0;
    }
}

void sequence_reset_mutates(struct Sequence *sequence) {
    size_t i;

    for (i = 0; i < sequence->node_size; ++i) {
        sequence->node[i].mutates = 0;
    }
}
