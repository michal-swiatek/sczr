#include <csignal>
#include <sched.h>

#include "common.h"
#include "CameraProcess.h"
#include "SharedMemory.h"
#include "SharedQueue.h"
#include "ImageProcess.h"
#include "GameProcess.h"
#include "LoggerProcess.h"

/* functions to support the main process */
void init_setup(); // creating shared objects
void start_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id); // starting children procs
void setup_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id); // apply scheduling policy
void print_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id); // print out children pids
void kill_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id); // kill children procs before finishing
void selectTestPreset();
void childMenu(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id); // option to quit the program
void unlinkObjects();


template <typename T>
pid_t runProcess();

enum class SchedulingPolicy {DEFAULT, FIFO, RR};

SchedulingPolicy schedPolicy = SchedulingPolicy::DEFAULT;
bool cpuCoreBound = false;

int main() {
    selectTestPreset();

    init_setup();
    pid_t cam_proc_id, image_proc_id, game_proc_id, log_proc_id;

    start_child_procs(cam_proc_id, image_proc_id, game_proc_id, log_proc_id);
    setup_child_procs(cam_proc_id, image_proc_id, game_proc_id);

    childMenu(cam_proc_id, image_proc_id, game_proc_id, log_proc_id);

    unlinkObjects();
    return 0;
}

void start_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id)
{
    cam_proc_id = runProcess<CameraProcess>();

    image_proc_id = runProcess<ImageProcess>();

    game_proc_id = runProcess<GameProcess>();

    log_proc_id = runProcess<LoggerProcess>();


}

void setup_child_procs(pid_t cam_proc_id, pid_t image_proc_id, pid_t game_proc_id)
{
    sched_param params{99};
    int result = 0;

    switch (schedPolicy) {
        case SchedulingPolicy::FIFO:
            result |= sched_setscheduler(cam_proc_id, SCHED_FIFO, &params);
            result |= sched_setscheduler(image_proc_id, SCHED_FIFO, &params);
            break;
        case SchedulingPolicy::RR:
            result |= sched_setscheduler(cam_proc_id, SCHED_RR, &params);
            result |= sched_setscheduler(image_proc_id, SCHED_RR, &params);
            break;
        case SchedulingPolicy::DEFAULT:
        default:
            break;
    }

    if (result)
    {
        std::cout << strerror(errno) << '\n';
        exit(1);
    }

    if (cpuCoreBound)
    {
        result = 0;
        cpu_set_t cpuSet;

        CPU_ZERO(&cpuSet);
        CPU_SET(1, &cpuSet);

        result |= sched_setaffinity(cam_proc_id, sizeof(cpu_set_t), &cpuSet);

        CPU_ZERO(&cpuSet);
        CPU_SET(2, &cpuSet);

        result |= sched_setaffinity(image_proc_id, sizeof(cpu_set_t), &cpuSet);

        if (result)
        {
            std::cout << strerror(errno) << '\n';
            exit(1);
        }
    }
}

void selectTestPreset()
{
    char c;

    do
    {
        std::cout << "q - Quit\n";
        std::cout << "1. Default scheduling, no cpu core bound\n";
        std::cout << "2. FIFO scheduling, no cpu core bound\n";
        std::cout << "3. Round Robin scheduling, no cpu core bound\n";
        std::cout << "4. FIFO scheduling, no cpu core bound\n";
        std::cout << "5. Round Robin scheduling, cpu core bound\n";
        std::cout << "Select test preset: ";

        std::cin >> c;

        switch (c) {
            case '1':
                return;
            case '2':
                schedPolicy = SchedulingPolicy::FIFO;
                return;
            case '3':
                schedPolicy = SchedulingPolicy::RR;
                return;
            case '4':
                schedPolicy = SchedulingPolicy::FIFO;
                cpuCoreBound = true;
                return;
            case '5':
                schedPolicy = SchedulingPolicy::RR;
                cpuCoreBound = true;
                return;
            default:
                break;
        }

    } while (c != 'q');

    exit(0);
}

void init_setup()
{
    unlinkObjects();
    sem_t *producer = sem_open(SEM_PROD_NAME, O_CREAT, 0660, 0);
    sem_t *consumer = sem_open(SEM_CONS_NAME, O_CREAT, 0660, 1);
    mqd_t mesq = mq_open(MY_Q, O_CREAT | O_RDWR | O_NONBLOCK, 0660, NULL);
    mqd_t log_q = mq_open(LOG_Q, O_CREAT | O_RDWR | O_NONBLOCK, 0660, NULL);
}

void print_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id)
{
    std::cout << "\n1. Camera Process pid: " << cam_proc_id;
    std::cout << "\n2. Image Process pid: " << image_proc_id;
    std::cout << "\n3. Game Process pid: " << game_proc_id;
    std::cout << "\n4. Logger Process pid: " << log_proc_id;
    std::cout << "\n\nUse included bash scripts to chanage proc parameters, use given pid\n";
}

void childMenu(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id)
{
    while(true)
    {
        print_child_procs(cam_proc_id, image_proc_id, game_proc_id, log_proc_id);
        std::cout<<"To end the program, please enter <q>: ";
        char c;
        std::cin>>c;
        if(c=='q')
        {
            kill_child_procs(cam_proc_id, image_proc_id, game_proc_id, log_proc_id);
            return;
        }
    }
}

void kill_child_procs(pid_t& cam_proc_id, pid_t& image_proc_id, pid_t& game_proc_id, pid_t& log_proc_id)
{
    kill(cam_proc_id, SIGTERM);
    kill(image_proc_id, SIGTERM);
    kill(game_proc_id, SIGTERM);
    kill(log_proc_id, SIGTERM);
}

void unlinkObjects()
{
    mq_unlink(MY_Q);
    mq_unlink(LOG_Q);
    shm_unlink(FILE_NAME);
    sem_unlink(SEM_CONS_NAME);
    sem_unlink(SEM_PROD_NAME);
}

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