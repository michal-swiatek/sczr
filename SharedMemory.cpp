//
// Created by miki on 16.12.2020.
//

#include "SharedMemory.h"

#include "common.h"

SharedMemory::SharedMemory(bool write)
{
    // create a shared memory buffer of size = size with a descriptor = shm_fd
    size = sizeof(Data);
    shm_fd = shm_open(FILE_NAME, O_CREAT | O_RDWR, 0660);
    ftruncate(shm_fd, size);
    data = write ? (Data*)mmap(nullptr, size, PROT_WRITE, MAP_SHARED, shm_fd, 0) : (Data*) mmap(nullptr, size, PROT_READ, MAP_SHARED, shm_fd, 0);

    // open semaphores for correct synchronization
    errno = 0;
    if((this->producer = sem_open(SEM_PROD_NAME, 0)) == SEM_FAILED) printf("%s", strerror(errno));
    if((this->consumer = sem_open(SEM_CONS_NAME, 0)) == SEM_FAILED) printf("%s", strerror(errno));
    if( errno != 0 ) exit(3);
}

SharedMemory::~SharedMemory()
{
    shm_unlink(FILE_NAME);
}
