#include <stdlib.h>
#include <memory.h>
#include <string.h>
#include "fsm.h"

enum State {
    PowerOn,

    S0, S1, S2, S3, S4, S5, S6, Sc,
    Se,

    Send,
    PowerOff
};

static void print_state(FILE *fd, enum State state);

enum Signal {
    TurnStepLeft = 0,                  // Один шаг склонения контейнера влево
    TurnStepRight,                     // Один шаг склонения контейнера вправо
    DoStepLeft,                        // Один шаг движение контейнера влево
    DoStepRight,                       // Один шаг движение контейнера вправо
    EndLittleSequence,                 // Окончание малого круга тройки
    NextLittleCircle,                  // Сигнал следующего круга тройки
    EndBigSequence,                    // Окончание большого круга
    NextBigCircle,                     // Сигнал следующей части большого круга

    ResetTimer,                        // Сброс таймера наполнения контейнера
    ResetTimer2,                       // --
    ResetTimer3,                       // --
    OpenGate,                          // Сиграл открытия задвижки резервуара
    OpenGate2,                         // --
    OpenGate3,                         // --
    CloseGate,                         // Сигнал закрытия задвижки резервуара
    CloseGate2,                        // --
    CloseGate3,                        // --

    ResetCounter,                      // Сброс счетчика малого круга троек

    End_OutputIndicator
};

#if 0
inline void memory_set_bit(uint8_t *mem, const uint8_t bit) {
    asm("bts %1,%0" : "+m" (*mem) : "r" (bit));
}
#else

inline void memory_set_bit(uint8_t *mem, const uint8_t bit) {
    *((uint64_t *) mem) |= (1 << bit);
}

#endif

void signal(struct FSM *fsm, enum Signal signal, uint8_t count);

enum Input {
    DeviceRunning = 0,                 // Взодной сигнал работы устройства
    StartLittleCircle,                 // Начало исполнения тройки
    DoLittleCircle,                    // Нужно ли исполнять
    LittleSequenceCounterOverflow,     // Счетчик малых троек переполнен
    BigSequenceCounterOverflow,        // Счетчик большой последовательности переполнен
    EndLeftTerminateSensor,            // Концевой датчик крайнего левого положения
    EndRightTerminateSensor,           // Концевой датчик крайнего правого положения
    TurnLeftTerminateSensor,           // Концевой датчик склонения контейнера влево
    TurnRightTerminateSensor,          // Концевой датчик склонения контейнера вправо
    TurnNormalTerminateSensor,         // Концевой датчик нормального положения контейнера

    TimerClockOverflow,                // Переполнен таймер наполнения контейнера
    TimerClockOverflow2,               // --
    TimerClockOverflow3,               // --
    PositionSensor,                    // Датчик положения контейнера над резервуаром или под бункером
    PositionSensor2,                   // --
    PositionSensor3,                   // --
    PositionSensor4,                   // --
    OpenGateSensor,                    // Датчик открытости резервуара
    OpenGateSensor2,                   // --
    OpenGateSensor3,                   // --

    End_InputIndicator
};

#if 0
inline bool memory_get_bit(const uint8_t *const mem, const uint8_t bit) {
    bool flag;
    asm("bt %2,%1; setb %0" : "=q" (flag) : "m" (*mem), "r" (bit));
    return flag;
}
#else

inline bool memory_get_bit(const uint8_t *const mem, const uint8_t bit) {
    return (*((uint64_t *) mem) >> bit) & 0x01;
}

#endif

bool input(struct FSM *fsm, enum Input in, uint8_t count);

bool read_input(struct FSM *fsm);

void print_input(struct FSM *fsm, FILE *fd);

struct FSM {
    FILE *fd;                          // Дескриптор открытого фала для чтения последовательности выходов для симуляции процесса работы автомата
    uint32_t tick;                     // Счетчик тиков
    uint8_t sequence[4];               // Исполняемая последовательность троек
    bool debug;

    // Память автомата
    enum State state;                  // Текущее состояние автомата
    uint8_t little_sequence_count;     // Счетчик малых трйки
    uint8_t big_sequence_count;        // Счетчик большой последовательности троек
    //
    char *error_message;
    // Регистры состояний
    uint64_t input;
    uint64_t output;
};

void read_sequence(uint8_t sequence[4], uint32_t input) {
    sequence[0] = sequence[1] = sequence[2] = sequence[3] = 0;
    input %= 10000;
    sequence[0] = input / 1000;
    input %= 1000;
    sequence[1] = input / 100;
    input %= 100;
    sequence[2] = input / 10;
    input %= 10;
    sequence[3] = input / 1;
}

struct FSM *fsm_create(char *filename, uint32_t sequence, bool debug) {
    struct FSM *fsm = (struct FSM *) calloc(sizeof(struct FSM), 1);
    fsm->fd = fopen(filename, "r");
    fsm->state = PowerOn;
    fsm->debug = debug;
    read_sequence(fsm->sequence, sequence);
    return fsm;
}

void fsm_tick(struct FSM *fsm) {
    if (fsm != 0) {
        fsm->tick++;
        //
        switch (fsm->state) {
            case PowerOn: {
                if (input(fsm, EndRightTerminateSensor, 0) == true)
                    fsm->state = S0;
                break;
            }
            case S0: {
                if (input(fsm, StartLittleCircle, 0) == true) {
                    fsm->state = S1;
                }
                break;
            }
            case S1: {
                if (input(fsm, DoLittleCircle, 0) == true) {
                    fsm->state = S2;
                } else {
                    fsm->state = Sc;
                    signal(fsm, EndLittleSequence, 0);
                    signal(fsm, NextLittleCircle, 0);
                }
                break;
            }
            case S2: {
                fsm->state = PowerOff;
                break;
            }
            case Sc: {
                if (input(fsm, LittleSequenceCounterOverflow, 0) == true) {
                    fsm->state = Send;
                } else {
                    fsm->state = S0;
                }
                break;
            }
            case Send: {
                fsm->state = PowerOff;
                break;
            }

        }
        //
        if (fsm->debug) {
            fprintf(stdout, "[%04d] cs: ", fsm->tick);
            print_state(stdout, fsm->state);
            fprintf(stdout, "\n");
            fflush(stdout);
        }
    }
}

void fsm_update(struct FSM *fsm) {
    if (fsm != 0 && fsm->fd != 0) {
        if (feof(fsm->fd) == 0) {
            read_input(fsm);
        } else {
            fclose(fsm->fd);
            fsm->fd = 0;
        }
    }
    if (fsm != 0 && fsm->debug == true) {
        print_input(fsm, stdout);
        fprintf(fsm->fd, "\n"), fflush(fsm->fd);
    }
}

bool fsm_eof(struct FSM *fsm) {
    if (fsm == 0)
        return false;
    return fsm->state == PowerOff || fsm->state == Se || input(fsm, DeviceRunning, 0) == false;
}

void fsm_destroy(struct FSM **fsm) {
    if (fsm != 0 && (*fsm) != 0) {
        if ((*fsm)->fd != 0)
            fclose((*fsm)->fd);
        free((*fsm));
        (*fsm) = 0;
    }
}

void print_state(FILE *fd, enum State state) {
    char *text = 0;
    if (fd == 0) {
        fd = stdout;
    }

    switch (state) {
        case PowerOn:
            text = "PowerOn";
            break;
        case PowerOff:
            text = "PowerOff";
            break;
        case S0:
            text = "S0";
            break;
        case S1:
            text = "S1";
            break;
        case S2:
            text = "S2";
            break;
        case S3:
            text = "S3";
            break;
        case S4:
            text = "S4";
            break;
        case S5:
            text = "S5";
            break;
        case S6:
            text = "S6";
            break;
        case Sc:
            text = "Sc";
            break;
        default:
            text = "Unknown";
            break;
    }

    if (text != 0) {
        fprintf(fd, "%s", text);
    }
}

void signal(struct FSM *fsm, enum Signal signal, uint8_t count) {
    if (count > 0) {
        memory_set_bit((uint8_t *) &fsm->output, signal + count);
    } else {
        memory_set_bit((uint8_t *) &fsm->output, signal);
    }
}

bool input(struct FSM *fsm, enum Input in, uint8_t count) {
    if (count > 0) {
        return memory_get_bit((const uint8_t *) &fsm->input, in + count);
    } else {
        return memory_get_bit((const uint8_t *) &fsm->input, in);
    }
}

bool read_input(struct FSM *fsm) {
    if (fsm != 0 && fsm->fd != 0) {
        if (feof(fsm->fd) == 0) {
            int read = 0;
            int i = 0;

            fsm->input = 0L;
            while (feof(fsm->fd) == 0 && i < End_InputIndicator) {
                read = fgetc(fsm->fd);
                if (read == '#') {
                    while (feof(fsm->fd) == 0 && fgetc(fsm->fd) != '\n');
                    continue;
                } else if (read == '1') {
                    memory_set_bit((uint8_t *) &fsm->input, i);
                    i++;
                } else if (read == '0') {
                    i++;
                } else if (read == ' ' || read == '\r' || read == '\n') {
                    continue;
                } else {
                    fprintf(stderr, "Ошибка последовательности\n");
                    return false;
                }
            }
            read = fgetc(fsm->fd);
            if (read != '\n' && read != EOF) {
                fprintf(stderr, "Ошибка последовательности. Слишком большая\n");
            }
            return true;
        }
    }
    return false;
}

void print_input(struct FSM *fsm, FILE *fd) {
    int i;
    for (i = 0; i < End_InputIndicator; ++i) {
        fprintf(fd, "%s", input(fsm, i, 0) == true ? "1" : "0");
    }
    fflush(fd);
}
