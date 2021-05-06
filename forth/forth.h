#ifndef FM_VM_H
#define FM_VM_H

struct Fm *
fm_create();

void
fm_inter(struct Fm *vm, const char * text);

void
fm_run(struct Fm *vm, unsigned int circles);

#endif