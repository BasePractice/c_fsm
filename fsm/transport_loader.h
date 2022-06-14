#ifndef C_FSM_TRANSPORT_LOADER_H
#define C_FSM_TRANSPORT_LOADER_H
#include <stdint.h>
#include <common.h>

#if defined(__cplusplus)
extern "C" {
#endif

struct Configuration {
    char version[10];
    char hash[36];

    struct MapMatrix map;
    struct MapMatrix things;
    struct MapMatrix paths;
};

bool
read_configuration(struct Configuration *configuration, const char *configuration_file);

#if defined(__cplusplus)
}
#endif

#endif //C_FSM_TRANSPORT_LOADER_H
