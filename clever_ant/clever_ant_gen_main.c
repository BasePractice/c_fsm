#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "sequence.h"
#include "clever_ant_gen.h"

/**
 * 3.0.0.1:3.0.0.2:3.0.0.3:3.0.0.4:3.0.2.0-315
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.0.0.4:3.0.2.0-295
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.4.0.4:3.0.2.0-275
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.0.0.4:3.0.2.0:3.4.2.4-275
 * 3.0.0.1:3.0.0.2:3.0.0.3:3.5.0.6:3.0.2.0:3.6.3.6:3.6.3.0-263
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.1.2.4-255
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4-255
 * 3.0.0.1:3.4.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.3.4:3.7.3.4:3.7.1.3-249
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4-235
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.4.2.0-231
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.4:3.4.0.6-231
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.6.3.0:3.4.2.4:3.2.3.4-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.2.3.6-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.0.2.6-227
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.6.2.6:3.7.3.0:3.4.2.6-227
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.3.3.4:3.4.2.4-227
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.8.3.0:3.4.2.4:3.3.3.4:3.4.2.4:3.0.1.3-223
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.6.0.9:3.0.2.0:3.4.2.4:3.9.2.9:3.7.3.3:3.2.3.0:3.9.3.0-223
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.11.0.6:3.0.2.0:3.6.2.6:3.4.2.0:3.12.1.12:3.12.0.1:3.9.1.1:3.1.0.8:3.10.2.6:3.6.1.4-223
 * 3.0.0.1:3.6.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.5.3.9:3.4.2.4:3.2.1.3:3.8.3.8:3.8.2.9:3.2.2.8-221
 * 3.0.0.1:3.5.0.2:3.0.0.3:3.5.0.4:3.7.3.9:3.6.2.6:3.4.2.0:3.0.0.11:3.10.3.3:3.8.0.1:3.1.2.0:3.4.1.10-219
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.1.2.8:3.5.3.12:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7-217
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.5.0.4:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7-215
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.3:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.1.3.12-205
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.1.3.12:3.3.3.3:3.13.0.12-201
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.3.12:3.3.3.3:3.13.0.12-197
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.0.2.0:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-197
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.4.2.4:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-187
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.1.2.8:3.8.0.11:3.7.1.17:3.10.3.2:3.5.1.2:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12-185
 * 3.0.0.1:3.13.0.2:3.0.0.3:3.15.0.12:3.2.3.7:3.2.2.13:3.9.2.19:3.8.0.11:3.7.1.17:3.10.3.2:3.8.3.4:3.5.1.6:3.6.2.0:3.7.2.6:3.3.0.7:3.15.2.4:3.3.3.3:3.13.0.12:3.18.0.2:3.9.1.17-183
 * */
/*http://www.demo.cs.brandeis.edu/papers/ep93.pdf */

static enum CircleStep next_cb(struct AntContext *ctx, void *userdata) {
    static uint64_t best_steps = 2000;

    if (ctx->steps < best_steps) {
        best_steps = ctx->steps;
        char *text = sequence_to_string(ctx->sequence);
        fprintf(stdout, "[%04llu] [%010llu gen] %s\n", best_steps, ctx->generation, text);
        fflush(stdout);
        free(text);
        return Best;
    }

    if (ctx->generation > 1000000000)
        return Complete;
    return Next;
}

int main() {
    struct Sequence sequence = {0, 0};

    srand(time(0));
    sequence_create(&sequence, "3.0.0.1:3.4.0.2:3.0.0.3:3.0.0.4:3.0.2.0");
    context_circle_sequence(&sequence, next_cb, 0);
    return EXIT_SUCCESS;
}