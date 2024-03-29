// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#include <stdlib.h>
#include <stdarg.h>
#include <assert.h>
#include <common.h>
#include "fsm_bulk.h"

struct FSM;

enum State {
    PowerOn,

    Ss,

    S0, S1, S2, S3, S4, S5, S6, Sc,
    Se,

    Send,
    PowerOff
};

static void print_state(struct FSM *fsm);

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

#endif

inline void fsm_signal(struct FSM *fsm, enum Signal signal, uint8_t count);

inline void fsm_reset_signal(struct FSM *fsm, enum Signal signal, uint8_t count);

bool output(struct FSM *fsm, enum Signal in, uint8_t count);

void print_output(struct FSM *fsm);

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

bool input(struct FSM *fsm, enum Input in, uint8_t count);

bool read_input(struct FSM *fsm);

void print_input(struct FSM *fsm);

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

    void (*print_std)(const char *format, ...);

    void (*print_err)(const char *format, ...);
};

static void fsm_default_print_std(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(stdout, format, args);
    va_end(args);
    fflush(stdout);
}

static void fsm_default_print_err(const char *format, ...) {
    va_list args;

    va_start(args, format);
    vfprintf(stderr, format, args);
    va_end(args);
    fflush(stderr);
}

void read_sequence(uint8_t sequence[4], uint32_t input) {
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
    if (0 == fsm)
        return 0;
    fsm->fd = fopen(filename, "r");
    fsm->state = PowerOn;
    fsm->debug = debug;
    fsm->print_std = fsm_default_print_std;
    fsm->print_err = fsm_default_print_err;
    read_sequence(fsm->sequence, sequence);
    return fsm;
}

void fsm_tick(struct FSM *fsm) {
    if (fsm != 0) {
        fsm->tick++;
        //
        switch (fsm->state) {
            case PowerOn: {
                fsm->state = Ss;
                break;
            }
            case Ss: {
                if (input(fsm, EndRightTerminateSensor, 0) == true) {
                    fsm_reset_signal(fsm, DoStepRight, 0);
                    fsm->state = S0;
                } else {
                    fsm_signal(fsm, DoStepRight, 0);
                }
                break;
            }
            case S0: {
                if (input(fsm, StartLittleCircle, 0) == true) {
                    fsm->state = S1;
                }
                break;
            }
            case S1: {
                bool r = memory_get_bit(
                        (uint8_t *) &fsm->sequence[fsm->big_sequence_count],
                        fsm->little_sequence_count);
                if (r) {
                    fsm->state = S2;
                } else {
                    fsm->state = Sc;
                    fsm_signal(fsm, EndLittleSequence, 0);
                    fsm_signal(fsm, NextLittleCircle, 0);
                }
                break;
            }
                /* Двигаемся налево пока не придем к нужному нам датчику */
            case S2: {
                if (input(fsm, PositionSensor, fsm->little_sequence_count) == true) {
                    fsm->state = S3;
                    fsm_signal(fsm, OpenGate, fsm->little_sequence_count);
                    fsm_reset_signal(fsm, DoStepLeft, 0);
                } else {
                    fsm_signal(fsm, DoStepLeft, 0);
                }
                break;
            }
                /* Смотрим когда у нас откроется залонка */
            case S3: {
                if (input(fsm, OpenGateSensor, fsm->little_sequence_count) == true) {
                    fsm_reset_signal(fsm, OpenGate, fsm->little_sequence_count);
                    fsm_signal(fsm, ResetTimer, fsm->little_sequence_count);
                    fsm->state = S4;
                }
                break;
            }
                /* Ожидает когда сработает таймер */
            case S4: {
                if (input(fsm, TimerClockOverflow, fsm->little_sequence_count) == true) {
                    fsm_reset_signal(fsm, ResetTimer, fsm->little_sequence_count);
                    fsm_signal(fsm, OpenGate, fsm->little_sequence_count);
                    fsm->state = S5;
                }
                break;
            }
                /* Закрытие залонки */
            case S5: {
                if (input(fsm, OpenGateSensor, fsm->little_sequence_count) == false) {
                    fsm_reset_signal(fsm, OpenGate, fsm->little_sequence_count);
                    fsm->state = S6;
                }
                break;
            }
                /* Завершение малого цикла */
            case S6: {
                fsm->state = Sc;
                fsm_signal(fsm, EndLittleSequence, 0);
                fsm_signal(fsm, NextLittleCircle, 0);
                break;
            }

            case Sc: {
                if (input(fsm, LittleSequenceCounterOverflow, 0) == true) {
                    fsm->state = Send;
                } else {
                    fsm->little_sequence_count++;
                    fsm->state = S0;
                    fsm_reset_signal(fsm, EndLittleSequence, 0);
                    fsm_reset_signal(fsm, NextLittleCircle, 0);
                }
                break;
            }
            case Send: {
                fsm->state = PowerOff;
                break;
            }
            case Se:
            case PowerOff:
                fsm->state = PowerOff;
                break;
        }
        //
        if (fsm->debug) {
            (*fsm->print_std)("[%04d] ", fsm->tick);
            print_input(fsm);
            (*fsm->print_std)("|");
            print_output(fsm);
            (*fsm->print_std)("| ");
            print_state(fsm);
            (*fsm->print_std)("\n");
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

void print_state(struct FSM *fsm) {
    static char combine_text[256];
    char *text;

    switch (fsm->state) {
        case PowerOn:
            text = "Включение питания";
            break;
        case PowerOff:
            text = "Выключение питания";
            break;
        case S0:
            text = "Начало малого цикла";
            break;
        case S1:
            sprintf(combine_text, "Исполнение малого цикла [%d]", fsm->little_sequence_count);
            text = combine_text;
            break;
        case S2:
            sprintf(combine_text, "Позиционирование [%d]", fsm->little_sequence_count);
            text = combine_text;
            break;
        case S3:
            sprintf(combine_text, "Открытие заслонки [%d]", fsm->little_sequence_count);
            text = combine_text;
            break;
        case S4:
            text = "Наполнение";
            break;
        case S5:
            sprintf(combine_text, "Закрытие заслонки [%d]", fsm->little_sequence_count);
            text = combine_text;
            break;
        case S6:
            text = "Завершение малого цикла";
            break;
        case Sc:
            text = "Проверка перехода в большой цикл";
            break;
        case Ss:
            text = "Поиск начального положения";
            break;
        default:
            text = "Unknown";
            break;
    }
    (*fsm->print_std)("%s", text);
}

void fsm_signal(struct FSM *fsm, enum Signal signal, uint8_t count) {
    if (count > 0) {
        memory_set_bit((uint8_t *) &fsm->output, signal + count);
    } else {
        memory_set_bit((uint8_t *) &fsm->output, signal);
    }
}

void fsm_reset_signal(struct FSM *fsm, enum Signal signal, uint8_t count) {
    if (count > 0) {
        memory_reset_bit((uint8_t *) &fsm->output, signal + count);
    } else {
        memory_reset_bit((uint8_t *) &fsm->output, signal);
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
            int read;
            int i = 0;

            fsm->input = 0L;
            while (feof(fsm->fd) == 0 && i < End_InputIndicator) {
                read = fgetc(fsm->fd);
                if ('#' == read) {
                    while (feof(fsm->fd) == 0 && fgetc(fsm->fd) != '\n');
                    continue;
                } else if ('1' == read) {
                    memory_set_bit((uint8_t *) &fsm->input, i);
                    i++;
                } else if ('0' == read) {
                    i++;
                } else if (' ' == read || '\r' == read || '\n' == read) {
                    continue;
                } else {
                    (*fsm->print_err)("Ошибка последовательности\n");
                    return false;
                }
            }
            read = fgetc(fsm->fd);
            if (read != '\n' && read != EOF) {
                (*fsm->print_err)("Ошибка последовательности. Слишком большая\n");
            }
            return true;
        }
    }
    return false;
}

void print_input(struct FSM *fsm) {
    int i;
    for (i = 0; i < End_InputIndicator; ++i) {
        (*fsm->print_std)("%s", input(fsm, i, 0) == true ? "1" : "0");
    }
}

bool output(struct FSM *fsm, enum Signal in, uint8_t count) {
    if (count > 0) {
        return memory_get_bit((const uint8_t *) &fsm->output, in + count);
    } else {
        return memory_get_bit((const uint8_t *) &fsm->output, in);
    }
}

void print_output(struct FSM *fsm) {
    int i;
    for (i = 0; i < End_OutputIndicator; ++i) {
        (*fsm->print_std)("%s", output(fsm, i, 0) == true ? "1" : "0");
    }
}
