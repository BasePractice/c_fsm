#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "forth.h"

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
#define DATA_SIZE          4096
#define CALL_SIZE          1024
#define ENTRY_SIZE         256

#define PORT_STDOUT        0

struct Entry {
    char *name;
    char *text;
    u32 uptr;
    u8 opcod;
    struct Entry *next;
};

struct Dict {
    struct Entry e[ENTRY_SIZE];
};

#define DATA_INTEGER 0
#define DATA_STRING  1

struct Fm {
    u32 base; /* Индекс базового адреса */
    u32 port;
    u8 memo[MEM_SIZE];
    u32 ip;
    u32 ic;
    u8 data[DATA_SIZE];
    i32 pdata;
    uptr call[CALL_SIZE];
    u32 pcall;

    struct Dict dict;

    void (*port_out)(struct Fm *vm, u32 port, u8 value);

    u8 (*port_in)(struct Fm *vm, u32 port);
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

static int
dict_hash(const char *const text) {
    int n = (int) strlen(text) - 1;
    int ret = 0;
    while (n >= 0)
        ret += text[n--];
    return ret;
}

static void
dict_add(struct Dict *dict, const char *const name, u32 address, u8 opcod) {
    const int hash = dict_hash(name);
    const int id = hash & (ENTRY_SIZE - 1);
    struct Entry *it = &dict->e[id];
    if (it != 0) {
        if (it->name != 0) {
            while (it) {
                if (!strcmp(it->name, name)) {
                    it->uptr = address;
                    it->opcod = opcod;
                    return;
                }
                if (it->next == 0)
                    break;
                it = it->next;
            }
            it->next = (struct Entry *) calloc(1, sizeof(struct Entry));
            it = it->next;
        }

        it->name = strdup(name);
        it->uptr = address;
        it->opcod = opcod;
    }
}

static struct Entry *
dict_find(struct Dict *dict, const char *const name) {
    const int hash = dict_hash(name);
    const int id = hash & (ENTRY_SIZE - 1);
    struct Entry *it = &dict->e[id];
    while (it && it->name) {
        if (!strcmp(it->name, name))
            return it;
    }
    return 0;
}

static __inline u8
data_peek(struct Fm *vm) {
    assert(vm->pdata >= 0);
    return vm->data[vm->pdata];
}

static __inline u8
data_pop(struct Fm *vm) {
    assert(vm->pdata >= 0);
    return vm->data[vm->pdata--];
}

static __inline void
data_push_integer(struct Fm *vm, u32 value) {
    vm->pdata += 1;
    *((u32 *) &vm->data[vm->pdata]) = value;
    vm->pdata += sizeof(u32);
    vm->data[vm->pdata] = DATA_INTEGER;
}

static __inline u32
data_peek_integer_without_type(struct Fm *vm) {
    return *((u32 *) &vm->data[vm->pdata - sizeof(u32) - 1 - 1]);
}

static __inline u32
data_pop_integer_without_type(struct Fm *vm) {
    u32 value;
    vm->pdata -= sizeof(u32) - 1;
    value = *((u32 *) &vm->data[vm->pdata]);
    vm->pdata -= 1;
    return value;
}

static __inline u32
data_pop_integer(struct Fm *vm) {
    u8 type = data_pop(vm);
    assert(type == DATA_INTEGER);
    return data_pop_integer_without_type(vm);
}

static __inline u32
data_peek_integer(struct Fm *vm) {
    u8 type = data_peek(vm);
    assert(type == DATA_INTEGER);
    return data_peek_integer_without_type(vm);
}

static __inline void
data_push_string(struct Fm *vm, u32 begin, u32 end) {
    u32 i;

    for (i = end - 1; i >= begin; i--) {
        vm->data[++vm->pdata] = *(vm->memo + i);
    }
    *((u32 *) &vm->data[++vm->pdata]) = end - begin;
    vm->pdata += sizeof(u32);
    vm->data[vm->pdata] = DATA_STRING;
}

static __inline u8
memo_read_u8(struct Fm *vm, u32 i) {
    return vm->memo[i];
}

static __inline void
memo_write_u8(struct Fm *vm, u32 i, u8 value) {
    vm->memo[i] = value;
}

static __inline u32
memo_read_u32(struct Fm *vm, u32 i) {
    return *((u32 *) &vm->memo[i]);
}

static __inline void
memo_write_u32(struct Fm *vm, u32 i, u32 value) {
    *((u32 *) &vm->memo[i]) = value;
}

static void
port_out(struct Fm *vm, u32 port, u8 value) {
    switch (port) {
        default:
        case PORT_STDOUT: {
            fprintf(stdout, "%c", value);
            break;
        }
    }
}

static u8
port_in(struct Fm *vm, u32 port) {
}

void
fm_run(struct Fm *vm, const unsigned int circles) {
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
            case BaseRead: {
                vm->base = data_pop_integer(vm);
                break;
            }
            case BaseWrite: {
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
                        u32 ptr = vm->pdata - sizeof(u32) - 1 - 1;
                        for (i = ptr, k = ptr + len + 1; i <= ptr + len; ++i) {
                            vm->data[k] = vm->data[i];
                        }
                        *((u32 *) &vm->data[vm->pdata]) = len;
                        vm->pdata += sizeof(u32);
                        vm->pdata += 1;
                        vm->data[vm->pdata] = DATA_STRING;
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
                            vm->pdata--;
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
                vm->call[vm->pcall++] = vm->ip;
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
                const u32 address = vm->call[--vm->pcall];
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
fm_default(struct Fm *vm) {
    dict_add(&vm->dict, "+", 0, Add);
    dict_add(&vm->dict, "-", 0, Sub);
    dict_add(&vm->dict, "/", 0, Div);
    dict_add(&vm->dict, "*", 0, Mul);
    dict_add(&vm->dict, "DROP", 0, Drop);
    dict_add(&vm->dict, "DUP", 0, Dup);
    dict_add(&vm->dict, ".", 0, Dot);
    dict_add(&vm->dict, "BASE_W", 0, BaseWrite);
    dict_add(&vm->dict, "BASE_R", 0, BaseRead);
    dict_add(&vm->dict, "IN", 0, PortIn);
    dict_add(&vm->dict, "OUT", 0, PortOut);
    dict_add(&vm->dict, ".\"", 0, PutString);
}

struct Fm *
fm_create() {
    struct Fm *vm = (struct Fm *) calloc(1, sizeof(struct Fm));
    memset(vm->memo, 0, sizeof(vm->memo));
    memset(vm->data, 0, sizeof(vm->data));
    memset(vm->call, 0, sizeof(vm->call));
    memset(&vm->dict, 0, sizeof(vm->dict));
    vm->pdata = -1;
    vm->pcall = 0;
    vm->ip = 0;
    vm->ic = 0;
    vm->base = 0;
    vm->port_in = port_in;
    vm->port_out = port_out;
    vm->port = PORT_STDOUT;
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
fm_inter(struct Fm *vm, const char *text) {
    char *p = (char *) text;
    char token[256];

    while (*p != 0) {
        p = tok_next(p, token);
        if (token[0] == 0)
            continue;
        if (token[0] == ':' && token[1] == 0) {
            char name[sizeof(token)];
            const u32 address = vm->ic;
            p = tok_next(p, token);
            strcpy(name, token);
            while (1) {
                p = tok_next(p, token);
                if (token[0] == ';' && token[1] == 0) {
                    memo_write_u8(vm, vm->ic++, Ret);
                    dict_add(&vm->dict, name, address, Nop);
                    break;
                } else if (token[0] == '(' && token[1] == 0) {
                    p = tok_next(p, token);
                    while (*p && !(token[0] == ')' && token[1] == 0)) {
                        p = tok_next(p, token);
                    }
                } else if (tok_isnumber(token)) {
                    const u32 n = strtol(token, 0, 10);
                    memo_write_u8(vm, vm->ic++, Push);
                    memo_write_u32(vm, vm->ic, n);
                    vm->ic += sizeof(u32);
                } else if (tok_isstring(token)) {
                    u32 len;
                    u32 d;
                    ++p;
                    p = tok_next_char(p, token, '"');
                    if (*p != 0)
                        ++p;
                    len = strlen(token);
                    memo_write_u8(vm, vm->ic++, PutString);
                    memo_write_u32(vm, vm->ic, len);
                    vm->ic += sizeof(u32);
                    for (d = 0; d < strlen(token); d++) {
                        memo_write_u8(vm, vm->ic++, token[d]);
                    }
                } else {
                    struct Entry *e = dict_find(&vm->dict, token);
                    if (e == 0) {
                        fprintf(stderr, "Function %s not found\n", token);
                        return;
                    }
                    if (e->opcod > Nop) {
                        memo_write_u8(vm, vm->ic++, e->opcod);
                    } else {
                        memo_write_u8(vm, vm->ic++, Call);
                        memo_write_u32(vm, vm->ic, e->uptr);
                        vm->ic += sizeof(u32);
                    }
                }
            }
        } else {
            struct Entry *e = dict_find(&vm->dict, token);
            if (e == 0) {
                if (tok_isnumber(token)) {
                    data_push_integer(vm, strtol(token, 0, 10));
                } else {
                    fprintf(stderr, "Function %s not found\n", token);
                    return;
                }
            } else {
                if (e->opcod > Nop) {
                    const u32 ret = vm->ip;
                    vm->ip = MEM_SIZE - 1;
                    memo_write_u8(vm, MEM_SIZE - 1, e->opcod);
                    fm_run(vm, 1);
                    vm->ip = ret;
                } else {
                    const u32 ret = vm->ip;
                    vm->call[vm->pcall++] = 0xffffffff;
                    vm->ip = e->uptr;
                    fm_run(vm, -1);
                    vm->ip = ret;
                }
            }
        }
    }
}

