#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>

inline void __attribute__((always_inline)) memory_set_bit(uint8_t *mem, const uint8_t bit) {
    *((uint16_t *) mem) |= (1 << bit);
}

inline void __attribute__((always_inline)) memory_reset_bit(uint8_t *mem, const uint8_t bit) {
    *((uint16_t *) mem) &= ~(1 << bit);
}

inline bool __attribute__((always_inline)) memory_get_bit(const uint8_t *const mem, const uint8_t bit) {
    return (*((uint64_t *) mem) >> bit) & 0x01;
}

enum Input {
    Input_KeyboardPushButton_1,
    Input_KeyboardPushButton_2,
    Input_KeyboardPushButton_3,
    Input_Dummy_0,
    Input_Dummy_1,
    Input_Dummy_2,
    Input_Dummy_3,
    Input_Dummy_4,
    Input_Dummy_5,
    Input_Dummy_6,
    Input_Dummy_7,
    Input_Dummy_8,
    Input_Dummy_9,
    Input_Dummy_10,
    Input_Dummy_11,
    Input_Dummy_12,
    Input_EndIndicator
};

#define BUTTON_TICK_COUNT 2
enum CtlState {
    Ctl_Work,
    Ctl_InMenu,
    Ctl_InMenu_Left,
    Ctl_InMenu_Right
};
static enum CtlState state = Ctl_Work;
#define KEYBOARD_BUTTON_1 0
#define KEYBOARD_BUTTON_2 1
#define KEYBOARD_BUTTON_3 2
struct Key {
    enum Input button_input;
    uint32_t button_tick;
    bool triggered;
} keys[] = {
        {Input_KeyboardPushButton_1, 0, false},
        {Input_KeyboardPushButton_2, 0, false},
        {Input_KeyboardPushButton_3, 0, false}
};
#define MENU_NAME_SIZE 6
struct MenuItem {
    char name[MENU_NAME_SIZE];
    void (*callback)(struct MenuItem *item);
    struct MenuItem *next;
};

bool fd_read_input(FILE *fd, uint16_t *input) {
    if (0 != fd) {
        if (feof(fd) == 0) {
            int read;
            int i = 0;

            (*input) = 0L;
            while (feof(fd) == 0 && i < Input_EndIndicator) {
                read = fgetc(fd);
                if ('#' == read) {
                    while (feof(fd) == 0 && fgetc(fd) != '\n');
                    continue;
                } else if ('1' == read) {
                    memory_set_bit((uint8_t *) input, i);
                    i++;
                } else if ('0' == read) {
                    i++;
                } else if (' ' == read || '\r' == read || '\n' == read || EOF == read) {
                    continue;
                } else {
                    fprintf(stderr, "Ошибка последовательности\n");
                    return false;
                }
            }
            read = fgetc(fd);
            if (read != '\n' && read != EOF) {
                fprintf(stderr, "Ошибка последовательности. Слишком большая\n");
            }
            return true;
        }
    }
    return false;
}

bool read_input(uint16_t input, enum Input in, uint8_t count) {
    if (count > 0) {
        return memory_get_bit((const uint8_t *) &input, in + count);
    } else {
        return memory_get_bit((const uint8_t *) &input, in);
    }
}

void print_input(FILE *output, uint16_t input) {
    int i;
    for (i = 0; i < Input_EndIndicator; ++i) {
        fprintf(output, "%s", read_input(input, i, 0) == true ? "1" : "0");
    }
}

void print_state(FILE *output) {
    switch (state) {
        case Ctl_InMenu: {
            fprintf(output, "MENU");
            break;
        }
        case Ctl_InMenu_Left: {
            fprintf(output, "MENU LEFT");
            break;
        }
        case Ctl_InMenu_Right: {
            fprintf(output, "MENU RIGHT");
            break;
        }
        case Ctl_Work: {
            fprintf(output, "WORK");
            break;
        }
        default: {
            fprintf(output, "UNKNOWN");
            break;
        }
    }
}

static void update_display(uint16_t input) {

}

static void update_keys(uint16_t input) {
    int i;

    for (i = 0; i < sizeof(keys) / sizeof(keys[0]); ++i) {
        if (read_input(input, keys[i].button_input, 0)) {
            ++keys[i].button_tick;
            keys[i].triggered = false;
        } else {
            if (keys[i].button_tick > BUTTON_TICK_COUNT) {
                keys[i].triggered = true;
            } else {
                keys[i].triggered = false;
            }
            keys[i].button_tick = 0;
        }
    }
}

static void update_tick(uint16_t input) {
    switch (state) {
        case Ctl_Work: {
            if (keys[KEYBOARD_BUTTON_2].triggered) {
                state = Ctl_InMenu;
            }
            break;
        }
        case Ctl_InMenu: {
            if (keys[KEYBOARD_BUTTON_1].triggered) {
                state = Ctl_InMenu_Left;
            } else if (keys[KEYBOARD_BUTTON_3].triggered) {
                state = Ctl_InMenu_Right;
            }
            break;
        }
        case Ctl_InMenu_Left:
            state = Ctl_InMenu;
            break;
        case Ctl_InMenu_Right:
            state = Ctl_InMenu;
            break;
        default:
            break;
    }
}

int main() {
    uint64_t tick = 0;  /* Тик */
    uint16_t input = 0; /* Регистр внешних сигналов*/
    FILE *fd = fopen("ctl.input.t", "r");
    if (0 == fd)
        return EXIT_FAILURE;
    while (fd_read_input(fd, &input)) {
        ++tick;
        fprintf(stdout, "[%04llu] ", tick);
        print_input(stdout, input);
        fprintf(stdout, " | ");
        update_keys(input);
        update_tick(input);
        update_display(input);
        print_state(stdout);
        fprintf(stdout, "\n");
    }
    fclose(fd);
    return EXIT_SUCCESS;
}

