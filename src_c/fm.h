#ifndef FM_VM_H
#define FM_VM_H

struct Fm *
fm_create();

void
fm_compile(struct Fm *vm, const char * text);

void
fm_run(struct Fm *vm, unsigned int circles);

#endif