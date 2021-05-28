// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef FM_VM_H
#define FM_VM_H

typedef unsigned char u8;
typedef unsigned int u32;
typedef int i32;
typedef unsigned long long u64;

#if defined(_M_IX86)
typedef u32                uptr;
#elif defined(_M_X64)
typedef u64 uptr;
#else
typedef u64 uptr;
#endif

#define MEM_SIZE           4096
#define DATA_SIZE          2048
#define CALL_SIZE          1024
#define ENTRY_SIZE         256
#define FUN_NAME_SIZE      10

#define PORT_STD        0

struct Entry {
    char name[FUN_NAME_SIZE];
    uptr uptr;
    u8 opcod;
    u32 code_size;
};

struct VM {
    u32 base; /* Индекс базового адреса */
    u32 port;
    u8 memo[MEM_SIZE];
    u32 memo_it;
    u32 ip;
    u8 data[DATA_SIZE];
    i32 data_it;
    uptr call[CALL_SIZE];
    u32 call_it;
    struct Entry entry[ENTRY_SIZE];
    u32 entry_it;

    void (*port_out)(struct VM *vm, u32 port, u8 value);

    u8 (*port_in)(struct VM *vm, u32 port);
};

typedef void (*write_to)(u8 *ptr, u32 size, void *user_data);

typedef u8 (*read_to)(u32 it, void *user_data);

typedef int (*eof_to)(u32 it, void *user_data);

struct VM;

void
vm_create(struct VM *);

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