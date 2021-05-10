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
    uptr uptr;
    u8 opcod;
};

typedef void (*write_to)(u8 *ptr, u32 size, void *user_data);

typedef u8 (*read_to)(u32 it, void *user_data);

typedef int (*eof_to)(u32 it, void *user_data);

struct VM;

struct VM *
vm_create();

void
vm_inter(struct VM *vm, const char *text);

void
vm_start(struct VM *vm, char *fun);

void
vm_exec(struct VM *vm, struct Entry *entry);

struct Entry *
vm_search(struct VM *vm, char *fun);

void
vm_run(struct VM *vm, unsigned int circles);

void
vm_memo_read(struct VM *vm, write_to write, void *user_data);

void
vm_memo_write(struct VM *vm, read_to read, eof_to eof, void *user_data);

#endif