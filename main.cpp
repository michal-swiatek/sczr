#include <iostream>
#include <unistd.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <string.h>
#include <semaphore.h>
#include <fcntl.h>
#include <mqueue.h>
#include <string>
#include <mqueue.h>
#include "SharedMemory.h"
#include "SharedQueue.h"
#include <cstdlib>
#include <random>





void storeImage(char*& buf, SharedMemory& shm);
void ImageProcess();
void CameraProcess();
void findCenter(SharedMemory& shm, MyMes* result);
void GameProcess();



int main() {
    mq_unlink(MY_Q);
    shm_unlink(FILE_NAME);
    sem_unlink(SEM_CONS_NAME);
    sem_unlink(SEM_PROD_NAME);
    errno = 0;
    sem_t *producer = sem_open(SEM_PROD_NAME, O_CREAT, 0660, 0);
    errno = 0;
    sem_t *consumer = sem_open(SEM_CONS_NAME, O_CREAT, 0660, 1);
    mqd_t mesq = mq_open(MY_Q, O_CREAT | O_RDWR, 0660, NULL);

    srand(time(0));

    pid_t cam_proc_id = fork();
    if(cam_proc_id == 0)
    {
        CameraProcess();
        return 0;
    }
    pid_t image_proc_id = fork();
    if(image_proc_id == 0)
    {
        ImageProcess();
        return 0;
    }
    GameProcess();
    return 0;
}

void CameraProcess() {
    srand(time(0));
    while (true)
    {
        SharedMemory shm = SharedMemory(FILE_PATH, true);
        char *frame = (char *) malloc(shm.getSize());
        for (int i = 0; i < shm.getSize(); ++i) {
//            frame[i] = dict[rand()%21];
            frame[i] = (char)(rand()%256);

        }
        shm.prodOperation(storeImage, frame, shm);
        //printf("Camera Process\n");
    }
}
void storeImage(char*& buf, SharedMemory& shm)
{
    //printf("\nCamProc\n");
    memcpy((void*)shm.buffer, (void*)buf, shm.getSize() );

}


void ImageProcess()
{
    SharedMemory shm = SharedMemory(FILE_PATH, false);
    SharedQueue send_q = SharedQueue(true);
    while(true)
    {
        MyMes* result = (MyMes*)malloc(sizeof(MyMes));
        shm.consOperation(findCenter, shm, result);
        send_q.sendMes(result);
        printf("\nSending coordinates: %d %d\n", result->x, result->y);
        //printf("Image Process\n");
    }
}

void findCenter(SharedMemory& shm, MyMes* result)
{
//    printf("\nImgProc pid: %d\n", getpid());
    char max = 0;
    result->x = 0;
    result->y = 0;
    for(int i = 0; i < shm.getSize(); i+=3)
    {
        if(i >= 921591) {
//            printf("%d: %d ", i, (int)shm.buffer[i]);
//            printf("%d: %d ", i+1, (int)shm.buffer[i+1]);
//            printf("%d: %d ", i+2, (int)shm.buffer[i+2]);
        }
        char curr = shm.buffer[i];
        if( curr > max)
        {
            max = curr;
            result->x = (i/3)%IMAGE_WIDTH;
            result->y = (i/3)/IMAGE_WIDTH;
        }
    }
}

void GameProcess() {
    SharedQueue rec_q = SharedQueue(false);
    MyMes *mes;
    MyMes *temp = (MyMes*)malloc(sizeof(MyMes));
    while (true)
    {
    temp = nullptr;

            rec_q.receiveMes(temp);

        mes = temp;
        printf("Receiving coordinates: %d, %d\n", mes->x, mes->y);
    }
    delete mes;
    delete temp;
}