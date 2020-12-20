//
// Created by miki on 19.12.2020.
//

#ifndef SYNCHROTEST_SHAREDQUEUE_H
#define SYNCHROTEST_SHAREDQUEUE_H

#include <mqueue.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>


#define MY_Q "/my_q"
#define MAX_MSG_SIZE 512
#define MAX_MESSAGES 5
#define MSG_BUFFER_SIZE MAX_MSG_SIZE + 10

typedef struct MyMes{
    int x, y;
}MyMes;

class SharedQueue {
public:
    explicit SharedQueue(bool write);
    void sendMes(MyMes* mes);
    void receiveMes(MyMes*& msg);
private:
    mqd_t my_q;
};


#endif //SYNCHROTEST_SHAREDQUEUE_H
