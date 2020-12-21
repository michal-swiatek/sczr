//
// Created by miki on 16.12.2020.
//

#ifndef SYNCHROTEST_SHAREDMEMORY_H
#define SYNCHROTEST_SHAREDMEMORY_H


#include <iostream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <cstring>
#include <semaphore.h>

#include "common.h"

class SharedMemory
{
public:
    template<typename Func, typename... Args>
    void consOperation(Func f, Args&&... args);

    template<typename Func, typename... Args>
    void prodOperation(Func f, Args&&... args);

    int getSize(){ return size; };
    unsigned char* buffer;
    SharedMemory(const char* file_path, bool write);
    ~SharedMemory();

private:
    int shm_fd;
    sem_t* consumer;
    sem_t* producer;

    int size;
};

template <typename Func, typename... Args>
void SharedMemory::consOperation(Func f, Args&&... args)
{
    sem_wait(this->producer);
    f(std::forward<Args>(args)...);
    sem_post(this->consumer);
}

template <typename Func, typename... Args>
void SharedMemory::prodOperation(Func f, Args&&... args)
{
    sem_wait(this->consumer);
    f(std::forward<Args>(args)...);
    sem_post(this->producer);
}

#endif //SYNCHROTEST_SHAREDMEMORY_H
