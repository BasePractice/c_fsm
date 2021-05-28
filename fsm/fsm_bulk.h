// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_FSM_BULK_H
#define C_FSM_FSM_BULK_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#define READ_PART(part, n) (((part) >> (n)) & 0x01)

#if defined(__cplusplus)
extern "C" {
#endif

inline void read_sequence(uint8_t sequence[4], uint32_t input);

struct FSM;

struct FSM *fsm_create(char *filename, uint32_t sequence, bool debug);

void fsm_tick(struct FSM *fsm);

void fsm_update(struct FSM *fsm);

bool fsm_eof(struct FSM *fsm);

void fsm_destroy(struct FSM **fsm);


#if defined(__cplusplus)
}
#endif

#endif //C_FSM_FSM_BULK_H
