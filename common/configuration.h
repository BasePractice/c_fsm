#ifndef FSM_COMMON_CONFIGURATION_H
#define FSM_COMMON_CONFIGURATION_H
#include <stdint.h>
#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct ConfigurationReader {
    void (*read_number)(void *ud, const char *name, int i_value, double d_value, bool enable);
    void (*read_range)(void *ud, const char *name, int i_minimum, double d_minimum, int i_maximum, double d_maximum, bool enable);
    void (*read_object)(void *ud, const char *object_name, const char *property_name, const char *property_value);
    void (*read_version)(void *ud, const char *version);

    void (*read_matrix_int_allocate)(void *ud, const char *name, int size);
    void (*read_matrix_int_put)(void *ud, const char *name, int x, int y, int v);

    void (*hash)(void *ud, const char *hash, size_t hash_size);
    void *ud;
};

bool configuration_read_memory(const char *text, size_t size, struct ConfigurationReader *reader);

bool configuration_read_file(const char *filename, struct ConfigurationReader *reader);

char *configuration_cwd();

#if defined(__cplusplus)
}
#endif

#endif //FSM_COMMON_CONFIGURATION_H
