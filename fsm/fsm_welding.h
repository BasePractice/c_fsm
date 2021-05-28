// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_FSM_WELDING_H
#define C_FSM_FSM_WELDING_H

#if defined(__cplusplus)
extern "C" {
#endif

enum InputSignal {
    WELDING_OFF = 0,
    WELDING_ON = 1
};

enum AutomateState {
    AUTOMATE_OFF = 0, /*Автомат выключен*/
    AUTOMATE_T_N, /*Насыщение*/
    AUTOMATE_WELDING, /*Сварка*/
    AUTOMATE_T_C, /*Спад*/
    AUTOMATE_COMPLETE,
};

struct Automate {
    enum AutomateState state;
    int time_tick; /*Внутренний счетчик*/
    int time_n; /*Время насыщения*/
    int time_c; /*Время спада*/
    double value; /*Значение параметра*/
};


enum AutomateParameter {
    AUTOMATE_I = 0, /*Ток сварки*/
    AUTOMATE_U, /*Напряжение дуги*/
    AUTOMATE_V, /*Скорость подачи проволоки*/
    AUTOMATE_COUNT
};


enum EngineState {
    ENGINE_WELDING_OFF = 0,
    ENGINE_WELDING_RAS_ON,
    ENGINE_WELDING_RUN_AUTOMATE,
    ENGINE_WELDING_RUNNING,
    ENGINE_WELDING_COMPLETE_AUTOMATE,
    ENGINE_WELDING_GAS_STABLE,
    ENGINE_WELDING_GAS_OFF,
    ENGINE_WELDING_COMPLETE
};

struct Engine {
    enum EngineState state;
    int time_tick; /*Внутренний счетчик*/
    int time_n; /*Время насыщения, газ*/
    int time_g; /*Время окончательной подачи газа*/
    int time_c; /*Время спада, газ*/
    struct Automate automate[3];
    double value; /*Расход газа*/
};

void engine_init(struct Engine *engine);

void engine_tick(struct Engine *engine, enum InputSignal signal);

void engine_reset(struct Engine *engine);


#if defined(__cplusplus)
}
#endif


#endif //C_FSM_FSM_WELDING_H
