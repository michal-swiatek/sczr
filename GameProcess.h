//
// Created by miki on 22.12.2020.
//

#ifndef SCZR_GAMEPROCESS_H
#define SCZR_GAMEPROCESS_H

#include "SharedQueue.h"
#include <chrono>
#include <iostream>

class GameProcess {
public:
    GameProcess(): game_q(false, true, MY_Q), log_q(true, true, LOG_Q) {};

    [[noreturn]] void run();
private:
    SharedQueue game_q;
    SharedQueue log_q;
};


#endif //SCZR_GAMEPROCESS_H
