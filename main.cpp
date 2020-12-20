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





void storeImage(unsigned char*& buf, SharedMemory& shm);
void ImageProcess();
void CameraProcess();
void findCenter(SharedMemory& shm, MyMes* result);
void GameProcess();

#define RL 50
#define GL 50
#define BL 50
#define DIST 900

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
        unsigned char *frame = (unsigned char *) malloc(shm.getSize());
        for (int i = 0; i < shm.getSize(); ++i) {
            frame[i] = rand()%256;
            //printf("%d ", frame[i]);
        }
        shm.prodOperation(storeImage, frame, shm);
        //printf("Camera Process\n");
    }
}
void storeImage(unsigned char*& buf, SharedMemory& shm)
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
        //printf("\nSending coordinates: %d %d\n", result->x, result->y);
        //printf("Image Process\n");
    }
}

void findCenter(SharedMemory& shm, MyMes* result)
{
//    printf("\nImgProc pid: %d\n", getpid());
    char max = 0;
    result->x = 0;
    result->y = 0;
    int x_max=-1, x_min=IMAGE_WIDTH, y_max=-1, y_min=IMAGE_HEIGHT;
    for(int i = 0; i < shm.getSize(); i+=3)
    {
        int x = (unsigned char)shm.buffer[i];
        int y = (unsigned char)shm.buffer[i+1];
        int z = (unsigned char)shm.buffer[i+2];
        int x_curr = (i/3)%IMAGE_WIDTH;
        int y_curr = (i/3)/IMAGE_WIDTH;
        if((x-RL)*(x-RL) + (y-GL)*(y-GL) + (z-BL)*(z-BL) <= DIST)
        {
            if(x_curr < x_min && y_curr < y_min)
            {
                x_min = x_curr;
                y_min = y_curr;
                continue;
            }
            if(x_curr > x_max && y_curr > y_max)
            {
                x_max = x_curr;
                y_max = y_curr;
                continue;
            }
        }
        result->x = (x_max+x_min)/2;
        result->y = (y_max+y_min)/2;

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