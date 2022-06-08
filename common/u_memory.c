#include <c_logger.h>
#include "u_memory.h"

void *
u_dbg_malloc(size_t size, const char *filename, int line) {
    void *p = calloc(1, size);
    log_debug("[malloc ] %16x[%lu bytes] on %s:%d", p, size, filename, line);
    return p;
}

void
u_dbg_free(void *p, const char *filename, int line) {
    log_debug("[free   ] %16x on %s:%d", p, filename, line);
    if (p != NULL)
        free(p);
}

void *
u_dbg_realloc(void *p, size_t size, const char *filename, int line) {
    void *n = realloc(p, size);
    log_debug("[realloc] %16x to %16 on %s:%d", p, n, filename, line);
    return n;
}
