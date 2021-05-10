#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "forth.h"

#define DATA_INTEGER 0
#define DATA_STRING  1

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

enum Instruct {
    Nop = 0x00,
    Ret = 0x01,
    Call = 0x02,
    Add = 0x03,
    Sub = 0x04,
    Mul = 0x05,
    Div = 0x06,
    Push = 0x10,
    Drop = 0x11,
    Dup = 0x12,
    Dot = 0x13,
    BaseWrite = 0x14,
    BaseRead = 0x15,
    PortOut = 0x16,
    PortIn = 0x17,
    PutString = 0xf0 /** PUT_STRING (len: 4) (CHAR: len) */
};

static void
dict_add(struct VM *vm, const char *const name, u32 address, u8 opcod) {
    vm->entry[vm->entry_it].name = strdup(name);
    vm->entry[vm->entry_it].uptr = address;
    vm->entry[vm->entry_it].opcod = opcod;
    vm->entry_it++;
}

static struct Entry *
dict_find(struct VM *vm, const char *const name) {
    u32 it;

    for (it = 0; it < vm->entry_it; ++it) {
        if (!strcmp(vm->entry[it].name, name))
            return &vm->entry[it];
    }
    return 0;
}

static __inline u8
data_peek(struct VM *vm) {
    assert(vm->data_it >= 0);
    return vm->data[vm->data_it];
}

static __inline u8
data_pop(struct VM *vm) {
    assert(vm->data_it >= 0);
    return vm->data[vm->data_it--];
}

static __inline void
data_push_integer(struct VM *vm, u32 value) {
    vm->data_it += 1;
    *((u32 *) &vm->data[vm->data_it]) = value;
    vm->data_it += sizeof(u32);
    vm->data[vm->data_it] = DATA_INTEGER;
}

static __inline u32
data_peek_integer_without_type(struct VM *vm) {
    return *((u32 *) &vm->data[vm->data_it - sizeof(u32) - 1 - 1]);
}

static __inline u32
data_pop_integer_without_type(struct VM *vm) {
    u32 value;
    vm->data_it -= sizeof(u32) - 1;
    value = *((u32 *) &vm->data[vm->data_it]);
    vm->data_it -= 1;
    return value;
}

static __inline u32
data_pop_integer(struct VM *vm) {
    u8 type = data_pop(vm);
    assert(type == DATA_INTEGER);
    return data_pop_integer_without_type(vm);
}

static __inline u32
data_peek_integer(struct VM *vm) {
    u8 type = data_peek(vm);
    assert(type == DATA_INTEGER);
    return data_peek_integer_without_type(vm);
}

static __inline void
data_push_string(struct VM *vm, u32 begin, u32 end) {
    u32 i;

    for (i = end - 1; i >= begin; i--) {
        vm->data[++vm->data_it] = *(vm->memo + i);
    }
    *((u32 *) &vm->data[++vm->data_it]) = end - begin;
    vm->data_it += sizeof(u32);
    vm->data[vm->data_it] = DATA_STRING;
}

static __inline u8
memo_read_u8(struct VM *vm, u32 i) {
    assert(i < MEM_SIZE);
    return vm->memo[i];
}

static __inline void
memo_write_u8(struct VM *vm, u32 i, u8 value) {
    assert(i < MEM_SIZE);
    vm->memo[i] = value;
}

static __inline u32
memo_read_u32(struct VM *vm, u32 i) {
    assert(i < MEM_SIZE);
    return *((u32 *) &vm->memo[i]);
}

static __inline void
memo_write_u32(struct VM *vm, u32 i, u32 value) {
    assert(i < MEM_SIZE);
    *((u32 *) &vm->memo[i]) = value;
}

static void
port_out(struct VM *vm, u32 port, u8 value) {
    switch (port) {
        default:
        case PORT_STD: {
            fprintf(stdout, "%c", value);
            fflush(stdout);
            break;
        }
    }
}

static u8
port_in(struct VM *vm, u32 port) {
    switch (port) {
        default:
        case PORT_STD: {
            return fgetc(stdin);
        }
    }
}

void
vm_run(struct VM *vm, unsigned int circles) {
    u32 it = 0;
    u8 is_running = 1;
    while (is_running) {
        switch (memo_read_u8(vm, vm->ip)) {
            case Add: {
                const u32 n1 = data_pop_integer(vm);
                const u32 n2 = data_pop_integer(vm);
                data_push_integer(vm, n1 + n2);
                vm->ip++;
                break;
            }
            case Sub: {
                const u32 n1 = data_pop_integer(vm);
                const u32 n2 = data_pop_integer(vm);
                data_push_integer(vm, n1 - n2);
                vm->ip++;
                break;
            }
            case Mul: {
                const u32 n1 = data_pop_integer(vm);
                const u32 n2 = data_pop_integer(vm);
                data_push_integer(vm, n1 * n2);
                vm->ip++;
                break;
            }
            case Div: {
                const u32 n1 = data_pop_integer(vm);
                const u32 n2 = data_pop_integer(vm);
                data_push_integer(vm, n1 / n2);
                vm->ip++;
                break;
            }
            case Push: {
                ++vm->ip;
                const u32 n = memo_read_u32(vm, vm->ip);
                vm->ip += 4;
                data_push_integer(vm, n);
                break;
            }
            case BaseWrite: {
                vm->base = data_pop_integer(vm);
                break;
            }
            case BaseRead: {
                data_push_integer(vm, vm->base);
                break;
            }
            case Dup: {
                u8 type = data_peek(vm);
                ++vm->ip;
                switch (type) {
                    default:
                    case DATA_INTEGER: {
                        data_push_integer(vm, data_peek_integer(vm));
                        break;
                    }
                    case DATA_STRING: {
#if 1
#warning "Duplicate function for string not implement yet"
#else
                        u32 i, k;
                        u32 len = data_peek_integer_without_type(vm);
                        u32 ptr = vm->data_it - sizeof(u32) - 1 - 1;
                        for (i = ptr, k = ptr + len + 1; i <= ptr + len; ++i) {
                            vm->data[k] = vm->data[i];
                        }
                        *((u32 *) &vm->data[vm->data_it]) = len;
                        vm->data_it += sizeof(u32);
                        vm->data_it += 1;
                        vm->data[vm->data_it] = DATA_STRING;
#endif
                        break;
                    }
                }
                break;
            }
            case Drop: {
                u8 type = data_pop(vm);
                ++vm->ip;
                switch (type) {
                    default:
                    case DATA_INTEGER: {
                        data_pop_integer_without_type(vm);
                        break;
                    }
                    case DATA_STRING: {
                        u32 i;
                        u32 len = data_pop_integer_without_type(vm);
                        for (i = 0; i < len; ++i) {
                            vm->data_it--;
                        }
                        break;
                    }
                }
                break;
            }
            case Dot: {
                u8 type = data_pop(vm);
                ++vm->ip;
                switch (type) {
                    default:
                    case DATA_INTEGER: {
                        fprintf(stdout, "%u", data_pop_integer_without_type(vm));
                        fflush(stdout);
                        break;
                    }
                    case DATA_STRING: {
                        u32 i;
                        u32 len = data_pop_integer_without_type(vm);
                        for (i = 0; i < len; ++i) {
                            u8 d = data_pop(vm);
                            (*vm->port_out)(vm, vm->port, d);
                        }
                        break;
                    }
                }
                break;
            }
            case Call: {
                ++vm->ip;
                const u32 address = memo_read_u32(vm, vm->ip);
                vm->ip += 4;
                vm->call[vm->call_it++] = vm->ip;
                vm->ip = address;
                break;
            }
            case PutString: {
                u32 len;

                ++vm->ip;
                len = memo_read_u32(vm, vm->ip);
                vm->ip += sizeof(u32);
                data_push_string(vm, vm->ip, vm->ip + len);
                vm->ip += len;
                break;
            }
            case Ret: {
                const u32 address = vm->call[--vm->call_it];
                vm->ip++;
                vm->ip = address;
                if (address == 0xffffffff) {
                    is_running = 0;
                }
                break;
            }
            case PortOut: {
                u32 port, value;
                ++vm->ip;
                port = data_pop_integer(vm);
                value = data_pop_integer(vm);
                (*vm->port_out)(vm, port, value);
                break;
            }
            case PortIn: {
                u32 port, value;
                ++vm->ip;
                port = data_pop_integer(vm);
                value = (*vm->port_in)(vm, port);
                data_push_integer(vm, value);
                break;
            }
        }
        ++it;

        if (it >= circles) {
            is_running = 0;
        }
    }
}

static void
fm_default(struct VM *vm) {
    dict_add(vm, "+", 0, Add);
    dict_add(vm, "-", 0, Sub);
    dict_add(vm, "/", 0, Div);
    dict_add(vm, "*", 0, Mul);
    dict_add(vm, "DROP", 0, Drop);
    dict_add(vm, "DUP", 0, Dup);
    dict_add(vm, ".", 0, Dot);
    dict_add(vm, "BASE_W", 0, BaseWrite);
    dict_add(vm, "BASE_R", 0, BaseRead);
    dict_add(vm, "IN", 0, PortIn);
    dict_add(vm, "OUT", 0, PortOut);
    dict_add(vm, ".\"", 0, PutString);
}

struct VM *
vm_create() {
    struct VM *vm = (struct VM *) calloc(1, sizeof(struct VM));
    memset(vm->memo, 0, sizeof(vm->memo));
    memset(vm->data, 0, sizeof(vm->data));
    memset(vm->call, 0, sizeof(vm->call));
    memset(&vm->entry, 0, sizeof(vm->entry));
    vm->data_it = -1;
    vm->call_it = 0;
    vm->entry_it = 0;
    vm->ip = 0;
    vm->memo_it = 0;
    vm->base = 0;
    vm->port_in = port_in;
    vm->port_out = port_out;
    vm->port = PORT_STD;
    fm_default(vm);
    return vm;
}

static __inline char *
tok_next(char *p, char *token) {
    int d = 0;

    while (*p && isspace((int) *p)) ++p;
    while (*p && !isspace((int) *p))
        token[d++] = *p++;
    token[d] = 0;
    return p;
}

static __inline char *
tok_next_char(char *p, char *token, const char ch) {
    int d = 0;

    while (*p && *p != ch)
        token[d++] = *p++;
    token[d] = 0;
    return p;
}

static __inline int
tok_isnumber(char *token) {
    const u32 n = strlen(token);
    u32 i = 0;

    if (!isdigit((int) token[0]))
        return 0;
    while (i < n) {
        if (!isdigit((int) token[i]) && token[i] != '.')
            return 0;
        --i;
    }
    return 1;
}

static __inline int
tok_isstring(const char *token) {
    return token[0] == '.' && token[1] == '"' && token[2] == 0;
}

void
vm_exec(struct VM *vm, struct Entry *entry) {
    if (entry->opcod > Nop) {
        const u32 ret = vm->ip;
        vm->ip = MEM_SIZE - 1;
        memo_write_u8(vm, MEM_SIZE - 1, entry->opcod);
        vm_run(vm, 1);
        vm->ip = ret;
    } else {
        const u32 ret = vm->ip;
        vm->call[vm->call_it++] = 0xffffffff;
        vm->ip = entry->uptr;
        vm_run(vm, -1);
        vm->ip = ret;
    }
}

void
vm_start(struct VM *vm, char *fun) {
    struct Entry *e = vm_search(vm, fun);
    if (e == 0) {
        if (tok_isnumber(fun)) {
            data_push_integer(vm, strtol(fun, 0, 10));
        } else {
            fprintf(stderr, "Function %s not found\n", fun);
            fflush(stderr);
            return;
        }
    } else {
        vm_exec(vm, e);
    }
}

struct Entry *
vm_search(struct VM *vm, char *fun) {
    return dict_find(vm, fun);
}

void
vm_inter(struct VM *vm, const char *text) {
    char *p = (char *) text;
    char token[256];

    while (*p != 0) {
        p = tok_next(p, token);
        if (token[0] == 0)
            continue;
        if (token[0] == ':' && token[1] == 0) {
            char name[sizeof(token)];
            const u32 address = vm->memo_it;
            p = tok_next(p, token);
            strcpy(name, token);
            while (1) {
                p = tok_next(p, token);
                if (token[0] == ';' && token[1] == 0) {
                    memo_write_u8(vm, vm->memo_it++, Ret);
                    dict_add(vm, name, address, Nop);
                    break;
                } else if (token[0] == '(' && token[1] == 0) {
                    p = tok_next(p, token);
                    while (*p && !(token[0] == ')' && token[1] == 0)) {
                        p = tok_next(p, token);
                    }
                } else if (tok_isnumber(token)) {
                    const u32 n = strtol(token, 0, 10);
                    memo_write_u8(vm, vm->memo_it++, Push);
                    memo_write_u32(vm, vm->memo_it, n);
                    vm->memo_it += sizeof(u32);
                } else if (tok_isstring(token)) {
                    u32 len;
                    u32 d;
                    ++p;
                    p = tok_next_char(p, token, '"');
                    if (*p != 0)
                        ++p;
                    len = strlen(token);
                    memo_write_u8(vm, vm->memo_it++, PutString);
                    memo_write_u32(vm, vm->memo_it, len);
                    vm->memo_it += sizeof(u32);
                    for (d = 0; d < strlen(token); d++) {
                        memo_write_u8(vm, vm->memo_it++, token[d]);
                    }
                } else {
                    struct Entry *e = vm_search(vm, token);
                    if (e == 0) {
                        fprintf(stderr, "Function %s not found\n", token);
                        fflush(stderr);
                        return;
                    }
                    /*Формирование шитого кода*/
                    if (e->opcod > Nop) {
                        memo_write_u8(vm, vm->memo_it++, e->opcod);
                    } else {
                        memo_write_u8(vm, vm->memo_it++, Call);
                        memo_write_u32(vm, vm->memo_it, e->uptr);
                        vm->memo_it += sizeof(u32);
                    }
                }
            }
        } else {
            vm_start(vm, token);
        }
    }
}

void
vm_memo_read(struct VM *vm, write_to write, void *user_data) {
    u32 offset;

    for (offset = 0; offset < vm->memo_it; ++offset) {
        (*write)(vm->memo + offset, sizeof(u8), user_data);
    }
}

void
vm_memo_write(struct VM *vm, read_to read, eof_to eof, void *user_data) {
    memset(vm->memo, 0, MEM_SIZE);
    memset(vm->data, 0, DATA_SIZE);
    vm->memo_it = 0;
    vm->data_it = -1;
    while (!(*eof)(vm->memo_it, user_data)) {
        vm->memo[vm->memo_it] = (*read)(vm->memo_it, user_data);
        vm->memo_it++;
    }
}
