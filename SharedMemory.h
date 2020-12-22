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
#include <chrono>

#include "common.h"


/*
 *   Shared memory object allows for a synchronous exchange of data between two processes - producer/consumer
 *   Memory synchronization only available when using the provided API:
 *      - consOperation
 *      - prodOperation
 */

struct Data
{
    byte buffer[WIDTH * HEIGHT * 3]{};
    std::chrono::system_clock::time_point timestamp{};
    int id;
};

class SharedMemory
{
public:
    SharedMemory(bool write);
    ~SharedMemory();

    template<typename Func, typename... Args>
    void consOperation(Func f, Args&&... args);

    template<typename Func, typename... Args>
    void prodOperation(Func f, Args&&... args);

    int getSize(){ return size; };

    Data* data;

private:
    int shm_fd; // shared memory file descriptor
    sem_t* consumer; // consumer semaphore
    sem_t* producer; // producer semaphore

    int size;
};

// use to execute consumer's operations - one at a time
template <typename Func, typename... Args>
void SharedMemory::consOperation(Func f, Args&&... args)
{
    sem_wait(this->producer);
    f(std::forward<Args>(args)...);
    sem_post(this->consumer);
}

// use to execute producer's operations - one at a time
template <typename Func, typename... Args>
void SharedMemory::prodOperation(Func f, Args&&... args)
{
    sem_wait(this->consumer);
    f(std::forward<Args>(args)...);
    sem_post(this->producer);
}

#endif //SYNCHROTEST_SHAREDMEMORY_H
