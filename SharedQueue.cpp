//
// Created by miki on 19.12.2020.
//

#include "SharedQueue.h"


SharedQueue::SharedQueue(bool write)
{
    errno = 0;

    my_q = write ? mq_open(MY_Q, O_WRONLY) : mq_open(MY_Q, O_RDONLY | O_NONBLOCK);

    printf(strerror(errno));
}

// for our application, we do not need to handle sending errors, because if data gets lost, it doesnt affect our system
void SharedQueue::sendMes(MyMes* mes)
{
    char buf[8192];
    memcpy(buf, mes, sizeof(mes));
    auto x = mq_send(my_q, (const char*) buf, sizeof(MyMes), NULL);
}

// if the receive failed, we return the nullptr and let the user declare further actions
void SharedQueue::receiveMes(MyMes*& msg) const {
    mq_attr mattr{};
    mq_getattr(my_q, &mattr);

    char buf[mattr.mq_msgsize+1];
    auto x = mq_receive(this->my_q, &buf[0], (mattr.mq_msgsize)+1, NULL);

    if (x != -1)
        memcpy(msg, buf, mattr.mq_msgsize);
    else
    {
        msg->x = -1;
        msg->y = -1;
    }
}
