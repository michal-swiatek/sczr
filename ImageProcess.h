//
// Created by miki on 21.12.2020.
//

#ifndef SCZR_IMAGEPROCESS_H
#define SCZR_IMAGEPROCESS_H

#include "SharedMemory.h"
#include "SharedQueue.h"
#include "common.h"
#include <cstdlib>

class ImageProcess {
public:
    ImageProcess() : shm(NULL, false), send_q(true) {};
    ~ImageProcess() = default;

    [[noreturn]] void run();

private:
    static void findCenter(SharedMemory& shm, MyMes* result);
    SharedMemory shm;
    SharedQueue send_q;
};


#endif //SCZR_IMAGEPROCESS_H
