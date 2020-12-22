//
// Created by miki on 19.12.2020.
//

#ifndef SYNCHROTEST_SHAREDQUEUE_H
#define SYNCHROTEST_SHAREDQUEUE_H

#include <mqueue.h>

#include "common.h"
#include <chrono>
#include <cstring>

struct GameMes{
    int x, y, id;
    std::chrono::system_clock::time_point timestamp{};
};

struct LogMes{
    int id;
    std::chrono::system_clock::time_point begin{};
    std::chrono::system_clock::time_point end{};
};

class SharedQueue {
public:
    explicit SharedQueue(bool write, bool non_block, const char* q_name);

    // for our application, we do not need to handle sending errors, because if data gets lost, it doesnt affect our system
    template <class T>
    void sendMes(T* mes)
    {
        mq_send(my_q, (const char*)mes, sizeof(T), 0);
    }

    // if the receive failed, we return the nullptr and let the user declare further actions
    template<class T>
    void receiveMes(T*& msg) const
    {
        // receive the message to the desired buffer
        char buf[bufferSize];
        auto result = mq_receive(this->my_q, &buf[0], bufferSize, 0);

        // copy the contents of the message into the buffer or return a nullptr if something went wrong
        if (result != -1)
            memcpy(msg, buf, sizeof(T));
        else
            msg = nullptr;
    }

private:
    mqd_t my_q; // queue descriptor
    size_t bufferSize; // max size of a message
};


#endif //SYNCHROTEST_SHAREDQUEUE_H
