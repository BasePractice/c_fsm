// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
#ifndef C_FSM_FSM_COIN_H
#define C_FSM_FSM_COIN_H

enum State {
    STATE_0 = 0,
    STATE_1 = 1,
    STATE_2 = 2,
    STATE_E = 65535
};

enum Input {
    COIN_1 = 1,
    COIN_3 = 3,
    COIN_5 = 5,
    COIN_10 = 10
};

enum Output {
    COUNT_0 = 0,
    COUNT_1 = 1,
    COUNT_2 = 2,
    COUNT_3 = 3,
    COUNT_4 = 4
};

struct CoinMachine {
    enum State state;
};

enum Output coin_machine_tick(struct CoinMachine *machine, enum Input input);

#endif //C_FSM_FSM_COIN_H
