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
#include <signal.h>

#include "CameraProcess.h"
#include "common.h"




void init_setup();
void start_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id);
void print_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id);
void kill_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id);
void childMenu(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id);

void ImageProcess();
void findCenter(SharedMemory& shm, MyMes* result);
void GameProcess();


#define RL 150
#define GL 255
#define BL 150
#define DIST 4

template <typename T>
pid_t runProcess()
{
    pid_t result = fork();

    if (result == 0) {
        T process;
        process.run();

        return 0;
    }
    else
        return result;
}


int main() {
    init_setup();
    pid_t cam_proc_id, image_proc_id, game_proc_id;

    cam_proc_id = runProcess<CameraProcess>();

    start_child_procs(cam_proc_id, image_proc_id, game_proc_id);
    childMenu(cam_proc_id, image_proc_id, game_proc_id);

    mq_unlink(MY_Q);
    shm_unlink(FILE_NAME);
    sem_unlink(SEM_CONS_NAME);
    sem_unlink(SEM_PROD_NAME);

    return 0;
}

void start_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id)
{
    image_proc_id = fork();
    if(image_proc_id == 0) ImageProcess();

    game_proc_id = fork();
    if(game_proc_id != 0) GameProcess();
}

void init_setup()
{
    mq_unlink(MY_Q);
    shm_unlink(FILE_NAME);
    sem_unlink(SEM_CONS_NAME);
    sem_unlink(SEM_PROD_NAME);
    sem_t *producer = sem_open(SEM_PROD_NAME, O_CREAT, 0660, 0);
    sem_t *consumer = sem_open(SEM_CONS_NAME, O_CREAT, 0660, 1);
    mqd_t mesq = mq_open(MY_Q, O_CREAT | O_RDWR | O_NONBLOCK, 0660, NULL);
}

void print_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id)
{
    std::cout << "\n1. Camera Process pid: " << cam_proc_id;
    std::cout << "\n2. Image Process pid: " << image_proc_id;
    std::cout << "\n3. Game Process pid: " << game_proc_id;
    std::cout << "\n\nUse included bash scripts to chanage proc parameters, use given pid\n";
}

void childMenu(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id)
{
    while(true)
    {
        print_child_procs(cam_proc_id, image_proc_id, game_proc_id);
        std::cout<<"To end the program, please enter <q>: ";
        char c;
        std::cin>>c;
        if(c=='q')
        {
            kill_child_procs(cam_proc_id, image_proc_id, game_proc_id);
            return;
        }
    }
}

void kill_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id)
{
    kill(cam_proc_id, SIGTERM);
    kill(image_proc_id, SIGTERM);
    kill(game_proc_id, SIGTERM);
}

void ImageProcess()
{
    SharedMemory shm = SharedMemory(FILE_PATH, false);
    SharedQueue send_q = SharedQueue(true);

    while(true)
    {
        MyMes result{};
        shm.consOperation(findCenter, shm, &result);
        send_q.sendMes(&result);
    }
}

void findCenter(SharedMemory& shm, MyMes* result)
{
    int x_max=-1, x_min=WIDTH, y_max=-1, y_min=HEIGHT;
    for(int i = 0; i < shm.getSize(); i+=3)
    {
        int r = (unsigned char)shm.buffer[i];
        int g = (unsigned char)shm.buffer[i+1];
        int b = (unsigned char)shm.buffer[i+2];

        int x_curr = (i/3)%WIDTH;
        int y_curr = (i/3)/WIDTH;

        if((r-RL)*(r-RL) + (g-GL)*(g-GL) + (b-BL)*(b-BL) <= DIST * DIST)
        {
            x_min = std::min(x_curr, x_min);
            y_min = std::min(y_curr, y_min);
            x_max = std::max(x_curr, x_max);
            y_max = std::max(y_curr, y_max);
        }
    }

    result->x = (x_max+x_min)/2;
    result->y = (y_max+y_min)/2;
}

void GameProcess() {
    SharedQueue rec_q = SharedQueue(false);
    MyMes *mes = (MyMes*)malloc(sizeof(MyMes));

    while (true)
    {
        rec_q.receiveMes(mes);
    }
}