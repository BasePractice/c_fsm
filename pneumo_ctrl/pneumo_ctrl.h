// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef LITTLE_COURSE_PNEUMO_CTRL_H
#define LITTLE_COURSE_PNEUMO_CTRL_H

#include <stdbool.h>

#if defined(__cplusplus)
extern "C" {
#endif

enum PneumoState {
    PneumoState_Init = 0,
    PneumoState_1, PneumoState_2, PneumoState_3,
    PneumoState_4, PneumoState_5, PneumoState_6,
    PneumoState_7, PneumoState_8, PneumoState_9,
    PneumoState_FatalException
};

#define PNEUMO_CYLINDER_SIGNAL_UP     0
#define PNEUMO_CYLINDER_SIGNAL_DOWN   1
struct PneumoCylinder {
    int input_signal[2];
    int output_signal;
};

#define PNEUMO_CYLINDER_Y1 0
#define PNEUMO_CYLINDER_Y2 1
struct PneumoEngine {
    enum PneumoState state;
    int timeout;
    int delay;
    int timeouts[PneumoState_FatalException];
    int delays[PneumoState_FatalException];
    struct PneumoCylinder cylinders[2];
#if defined(PNEUMO_DEBUG)
    int line;
#endif
};

void pneumo_engine_init(struct PneumoEngine *engine);

bool pneumo_engine_tick(struct PneumoEngine *engine);

void pneumo_engine_destroy(struct PneumoEngine *engine);

#if defined(__cplusplus)
}
#endif

#endif //LITTLE_COURSE_PNEUMO_CTRL_H
