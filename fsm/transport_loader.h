#ifndef C_FSM_TRANSPORT_LOADER_H
#define C_FSM_TRANSPORT_LOADER_H
#include <stdint.h>
#include <common.h>

struct Configuration {
    char version[10];
    char hash[36];

    struct MapMatrix map;
};

bool
read_configuration(struct Configuration *configuration, const char *configuration_file);

#endif //C_FSM_TRANSPORT_LOADER_H
