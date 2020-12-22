//
// Created by miki on 22.12.2020.
//

#ifndef SCZR_LOGGERPROCESS_H
#define SCZR_LOGGERPROCESS_H

#include <mqueue.h>
#include <fstream>
#include <chrono>
#include "SharedQueue.h"
#include <iostream>

class LoggerProcess {
public:
    LoggerProcess(): log_q(false, false, LOG_Q){};

    [[noreturn]] void run();
private:
    SharedQueue log_q;
    std::ofstream output, output2;
};


#endif //SCZR_LOGGERPROCESS_H
