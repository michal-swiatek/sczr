#include <csignal>

#include "common.h"
#include "CameraProcess.h"
#include "SharedMemory.h"
#include "SharedQueue.h"
#include "ImageProcess.h"

/* functions to support the main process */
void init_setup(); // creating shared objects
void start_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id); // starting children procs
void print_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id); // print out children pids
void kill_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id); // kill children procs before finishing
void childMenu(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id); // option to quit the program

void GameProcess();

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
    cam_proc_id = runProcess<CameraProcess>();

    image_proc_id = runProcess<ImageProcess>();

    game_proc_id = fork();
    if(game_proc_id == 0) GameProcess();
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


void GameProcess() {
    SharedQueue rec_q = SharedQueue(false);
    auto* mes = (GameMes*)malloc(sizeof(GameMes));

    while (true)
    {
        auto* temp = mes;
        rec_q.receiveMes(temp);

#ifndef NDEBUG
        if (temp)
            std::cout << "GameProcess (running): " << mes->x << ' ' << mes->y << '\n';
#endif
    }
}