//
// Created by miki on 19.12.2020.
//

#include "SharedQueue.h"


SharedQueue::SharedQueue(bool write) {

    //mq_unlink(MY_Q);
    errno = 0;
    struct mq_attr* mattr = (mq_attr*)malloc(sizeof(struct mq_attr));
    mattr->mq_msgsize = sizeof(char)*32;
    mattr->mq_flags = 0;
    mattr->mq_maxmsg = 2;
    mattr->mq_curmsgs = 0;
    my_q = write == true ? mq_open (MY_Q, O_RDWR) : mq_open (MY_Q,  O_RDWR);

    printf(strerror(errno));
}

// for our application, we do not need to handle sending errors, because if data gets lost, it doesnt affect our system
void SharedQueue::sendMes(MyMes* mes)
{
    char* buf = (char*)malloc(8192);
    memcpy((void*) buf, (void*)mes, sizeof(mes));
    auto x = mq_send (my_q, (const char*) buf, sizeof(MyMes), NULL);
    delete buf;
}

// if the receive failed, we return the nullptr and let the user declare further actions
void SharedQueue::receiveMes(MyMes*& msg) {
    errno = 0;
    struct mq_attr* mattr = (mq_attr*)malloc(sizeof(struct mq_attr));
    char buf[mattr->mq_msgsize+1];
    auto x = mq_receive(this->my_q, &buf[0], (mattr->mq_msgsize)+1, NULL);
    if (x != -1)
    {
        msg = (MyMes *) buf;
        memcpy((void*) msg, (void*) buf, 8192);
        delete mattr;
        return;
    }
    delete mattr;
    msg = nullptr;
}
