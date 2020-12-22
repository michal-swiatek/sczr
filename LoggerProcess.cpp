//
// Created by miki on 22.12.2020.
//

#include "LoggerProcess.h"


[[noreturn]] void LoggerProcess::run() {
    // open for storing
    output.open(LOG_FILE_PATH);

    LogMes mes{};
    int i = 60;
    while (i-->0) {
        auto *temp = &mes;
        log_q.receiveMes(temp);
        auto latency = std::chrono::duration_cast<std::chrono::milliseconds>(mes.end - mes.begin).count();
        if (temp && output.good()) {
            output << mes.id << " " << latency << "\n";
#ifndef NDEBUG
            std::cout << "LoggerProcess (running): " << mes.id << ' ' << latency << '\n';
#endif
        }

    }
    output.close();
}