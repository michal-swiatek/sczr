//
// Created by miki on 19.12.2020.
//

#ifndef SYNCHROTEST_SHAREDQUEUE_H
#define SYNCHROTEST_SHAREDQUEUE_H

#include <mqueue.h>

#include "common.h"

struct GameMes{
    int x, y;
};

class SharedQueue {
public:
    explicit SharedQueue(bool write);

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
        char buf[bufferSize];
        auto x = mq_receive(this->my_q, &buf[0], bufferSize, 0);

        if (x != -1)
            memcpy(msg, buf, sizeof(T));
        else
            msg = nullptr;
    }

private:
    mqd_t my_q;
    size_t bufferSize;
};


#endif //SYNCHROTEST_SHAREDQUEUE_H
