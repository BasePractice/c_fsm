#include <configuration.h>
#include <string.h>
#include "transport_loader.h"

static void read_matrix_int_allocate(void *ud, const char *name, int size) {
    struct Configuration *p = (struct Configuration *) ud;
    if (strcmp(name, "map") == 0) {
        memset(&p->map, 0, sizeof(struct MapMatrix));
        matrix_allocate(&p->map, size);
    } else if (strcmp(name, "things") == 0) {
        memset(&p->things, 0, sizeof(struct MapMatrix));
        matrix_allocate(&p->things, size);
    } else if (strcmp(name, "paths") == 0) {
        memset(&p->paths, 0, sizeof(struct MapMatrix));
        matrix_allocate(&p->paths, size);
    }
}

static void read_matrix_int_put(void *ud, const char *name, int x, int y, int value) {
    struct Configuration *p = (struct Configuration *) ud;
    if (strcmp(name, "map") == 0) {
        size_t v = value;
        matrix_put(&p->map, x, y, (MapMatrixValue) v);
    } else if (strcmp(name, "things") == 0) {
        size_t v = value;
        matrix_put(&p->things, x, y, (MapMatrixValue) v);
    } else if (strcmp(name, "paths") == 0) {
        size_t v = value;
        matrix_put(&p->paths, x, y, (MapMatrixValue) v);
    }
}

static void read_number(void *ud, const char *name, int i_value, double d_value, bool enable) {
    struct Configuration *p = (struct Configuration *) ud;
    (void)p;
    (void)name;
    (void)i_value;
    (void)d_value;
    (void)enable;
}

static void
read_range(void *ud, const char *name, int i_minimum, double d_minimum, int i_maximum, double d_maximum, bool enable) {
    struct Configuration *p = (struct Configuration *) ud;
    (void)p;
    (void)name;
    (void)i_maximum;
    (void)i_minimum;
    (void)d_maximum;
    (void)d_minimum;
    (void)enable;
}

static void read_version(void *ud, const char *version) {
    struct Configuration *p = (struct Configuration *) ud;
    strncpy(p->version, version, sizeof(p->version));
}

static void configuration_hash(void *ud, const char *hash, size_t hash_size) {
    struct Configuration *p = (struct Configuration *) ud;
    if (hash_size < sizeof(p->hash)) {
        strncpy(p->hash, hash, hash_size);
    }
}

bool
read_configuration(struct Configuration *configuration, const char *configuration_file) {
    struct ConfigurationReader reader = {read_number, read_range, 0, read_version,
            read_matrix_int_allocate, read_matrix_int_put, configuration_hash, configuration};

    strcpy(configuration->version, "unknown");
    memset(configuration->hash, 0, sizeof(configuration->hash));
    return configuration_read_file(configuration_file, &reader);
}

static bool __attribute__((unused))
read_configuration_from_memory(struct Configuration *configuration, const char *configuration_file) {
    struct ConfigurationReader reader = {read_number, read_range, 0, read_version,
            read_matrix_int_allocate, read_matrix_int_put, configuration_hash, configuration};

    strcpy(configuration->version, "unknown");
    memset(configuration->hash, 0, sizeof(configuration->hash));
    return configuration_read_memory(configuration_file, strlen(configuration_file), &reader);
}

