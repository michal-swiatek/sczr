//
// Created by miki on 19.12.2020.
//

#include "SharedQueue.h"

#include <cerrno>
#include <cstring>
#include <iostream>

SharedQueue::SharedQueue(bool write)
{
    errno = 0;

    my_q = write ? mq_open(MY_Q, O_WRONLY) : mq_open(MY_Q, O_RDONLY | O_NONBLOCK);
    mq_attr mattr{};
    mq_getattr(my_q, &mattr);

    bufferSize = mattr.mq_msgsize;

    std::cout << strerror(errno);
}
