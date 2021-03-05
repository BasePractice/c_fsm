#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sequence.h"
#include "clever_ant_gen.h"

/*http://www.demo.cs.brandeis.edu/papers/ep93.pdf */

#define COND ctx->steps < best_steps
//#define COND ctx->steps < 2000

static enum CircleStep next_cb(struct AntContext *ctx, void *userdata) {
    static uint64_t best_steps = 2000;
    size_t i;

    if (COND) {
        best_steps = ctx->steps;
//        sequence_format(ctx->sequence, FormatEnters);
        sequence_format(ctx->sequence, FormatLinks);
        char *text = sequence_to_string(ctx->sequence);
        fprintf(stdout, "[%04llu] [%010llu gen] %s\n", best_steps, ctx->generation, text);
        for (i = 0; i < ctx->sequence->node_size; ++i) {
            fprintf(stdout, "[%04llu]:[%05d, %010d]\n", i,
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

int main(int argc, char **argv) {
    struct Sequence sequence = {0, 0};
    int strategy = MutateEatNext | MutateNotNext | MutateNotDo | MutateAllElements;

    if (argc < 2) {
        fprintf(stderr, "Not input sequence\n");
        return EXIT_FAILURE;
    }

    srand(time(0));
    sequence_create(&sequence, argv[1]);
    context_circle_sequence(&sequence, strategy, next_cb, 0);
    return EXIT_SUCCESS;
}