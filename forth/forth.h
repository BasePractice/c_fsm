#ifndef FM_VM_H
#define FM_VM_H

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long long u64;

#if defined(_M_IX86)
typedef u32                uptr;
#elif defined(_M_X64)
typedef u64                uptr;
#else
typedef u64 uptr;
#endif

#define MEM_SIZE           4096
#define DATA_SIZE          2048
#define CALL_SIZE          1024
#define ENTRY_SIZE         256

#define PORT_STD        0

struct Entry {
    char *name;
    char *text;
    uptr uptr;
    u8 opcod;
    struct Entry *next;
};

typedef void (*write_to)(u8 *ptr, u32 size, void *user_data);
typedef u8 (*read_to)(u32 it, void *user_data);
typedef int (*eof_to)(u32 it, void *user_data);

struct Fm *
fm_create();

void
fm_inter(struct Fm *vm, const char *text);

void
fm_start(struct Fm *vm, char *fun);

void
fm_exec(struct Fm *vm, struct Entry *entry);

struct Entry *
fm_search(struct Fm *vm, char *fun);

void
fm_run(struct Fm *vm, unsigned int circles);

void
fm_memo_read(struct Fm *vm, write_to write, void *user_data);

void
fm_memo_write(struct Fm *vm, read_to read, eof_to eof, void *user_data);

#endif