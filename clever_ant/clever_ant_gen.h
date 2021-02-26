#ifndef C_FSM_CLEVER_ANT_GEN_H
#define C_FSM_CLEVER_ANT_GEN_H

#if defined(__cplusplus)
extern "C" {
#endif

uint64_t context_next_run(const char *sequence);

char *context_get_do_name(int exec);

#if defined(__cplusplus)
}
#endif

#endif //C_FSM_CLEVER_ANT_GEN_H
