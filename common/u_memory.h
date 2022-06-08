#ifndef COMMON_U_MEMORY_H
#define COMMON_U_MEMORY_H

#include <stdlib.h>

#if defined(DETECT_MEMORY_LEAK)
#define u_malloc(size)    u_dbg_malloc((size), __FILE__, __LINE__)
#define u_calloc(size, count) u_dbg_malloc((size) * (count), __FILE__, __LINE__)
#define u_realloc(p, size) u_dbg_realloc((p), (size), __FILE__, __LINE__)
#define u_free(p)         u_dbg_free((p), __FILE__, __LINE__)
#else
#define u_malloc(size)           malloc((size))
#define u_calloc(size, count)    calloc((size), (count))
#define u_realloc(p, size)       realloc((p), (size))
#define u_free(p)         free((p))
#endif

#if defined(__cplusplus)
extern "C" {
#endif

void *u_dbg_malloc(size_t size, const char *filename, int line);

void *u_dbg_realloc(void *p, size_t size, const char *filename, int line);

void u_dbg_free(void *p_memory, const char *filename, int line);

#if defined(__cplusplus)
}
#endif

#endif //COMMON_U_MEMORY_H
