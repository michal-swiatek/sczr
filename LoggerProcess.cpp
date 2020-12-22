//
// Created by miki on 22.12.2020.
//

#include "LoggerProcess.h"


[[noreturn]] void LoggerProcess::run() {
    // open for storing
    output.open(LOG_FILE_PATH);

    // set the total amount of gathered samples
    int count = LOG_COUNT;

    LogMes mes{};

    while (count-->0) {

        // receive a message from shared message queue
        auto *temp = &mes;
        log_q.receiveMes(temp);

        if (temp && output.good()) {

            // calculate the time it took the data to reach the game process
            auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(mes.end - mes.begin).count();

            // store data into the logger's file
            output << mes.id << " " << latency << "\n";
#ifndef NDEBUG
            std::cout << "LoggerProcess (running): " << mes.id << ' ' << latency << '\n';
#endif
        }
    }
    output.close();
}