#ifndef C_FSM_ANT_H
#define C_FSM_ANT_H
#include <stdlib.h>
#include <stdint.h>

#define SQUARE_SIZE 32

#define SQUARE_EMPTY 0
#define SQUARE_APPLE 1
#define SQUARE_PATH  2

#define DIRECT_UP      1
#define DIRECT_RIGHT   2
#define DIRECT_BOTTOM  3
#define DIRECT_LEFT    4

extern const uint8_t APPLES;
extern const uint8_t TORUS[SQUARE_SIZE][SQUARE_SIZE];

void *ant_malloc_debug(size_t size, const char *fun, int line);
void *ant_realloc_debug(void *p, size_t size, const char *fun, int line);
void ant_free_debug(void *p, const char *fun, int line);

#if defined(MEMORY_DEBUG)

#define ant_malloc(size) ant_malloc_debug((size), __FUNCTION__, __LINE__)
#define ant_realloc(p, size) ant_realloc_debug((p), (size), __FUNCTION__, __LINE__)
#define ant_free(p) ant_free_debug((p), __FUNCTION__, __LINE__)


#else

#define ant_malloc(p) malloc((p))
#define ant_realloc(p, s) realloc((p), (s))
#define ant_free(p) free((p))

#endif


#endif //C_FSM_ANT_H
