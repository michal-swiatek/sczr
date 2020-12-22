//
// Created by miki on 19.12.2020.
//

#include "SharedQueue.h"

#include <cerrno>
#include <cstring>
#include <iostream>

SharedQueue::SharedQueue(bool write, bool non_block, const char* q_name)
{
    // open the correct queue - producers should call with write = true
    my_q = write ? ( non_block ? mq_open(q_name, O_WRONLY | O_NONBLOCK) : mq_open(q_name, O_WRONLY) ) : (non_block ? mq_open(q_name, O_RDONLY | O_NONBLOCK) : mq_open(q_name, O_RDONLY ) );

    // get the default msg size
    mq_attr mattr{};
    mq_getattr(my_q, &mattr);
    bufferSize = mattr.mq_msgsize;
}
