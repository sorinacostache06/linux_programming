#include "shared_resource.h"
#include <errno.h>
#include <wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>

#define MAX_PID 100

int pids[100];
sem_t sem;
struct region *addr;
union semun {
            int val;
            struct semid_ds *buf;
            unsigned short  *array;
};

void put_randdata()
{
    int r;
    srand(time(NULL));

    for (int i = 0; i < 5; i++) {
        r = rand() % 1000 + 1;
        char c[3];
        sprintf(c, "%d", r);
        strcat(addr->buf, c);
    }
}

int reserveSem(int semId, int semNum)
{
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = -1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    while (semop(semId, &sops, 1) == -1)
        if (errno != EINTR || !bsRetryOnEintr)
            return -1;
    return 0;
}

int releaseSem(int semId, int semNum)
{
    struct sembuf sops;
    sops.sem_num = semNum;
    sops.sem_op = 1;
    sops.sem_flg = bsUseSemUndo ? SEM_UNDO : 0;
    return semop(semId, &sops, 1);
}

int initSemAvailable(int semId, int semNum)
{
    union semun arg;
    arg.val = 1;
    return semctl(semId, semNum, SETVAL, arg);
}

int initSemInUse(int semId, int semNum)
{
    union semun arg;
    arg.val = 0;
    return semctl(semId, semNum, SETVAL, arg);
}

int main(int argc, char *argv[])
{
    int fd, semid;
    char *path;

    if (argc < 2){
        printf("error: no arguments\n");
        return -1;
    } else if (argc > 2) {
        printf("error: too many arguments\n");
        return -1;
    }

    path = argv[1];

    if ((semid = semget(SEM_KEY, 2, IPC_CREAT | OBJ_PERMS)) == -1) {
        printf("Semaphore get no error: %d %s \n", errno, strerror(errno));
        return -1; 
    }

    if (initSemAvailable(semid, WRITE_SEM) == -1) {
        printf("Semaphore ctl1 no error: %d %s \n", errno, strerror(errno));
        return -1; 
    }
    if (initSemInUse(semid, READ_SEM) == -1) {
        printf("Semaphore ctl2 no error: %d %s \n", errno, strerror(errno));
        return -1; 
    }

    if ((fd = shm_open("my_region", O_CREAT | O_RDWR, S_IRUSR | S_IRWXU)) == -1) {
        printf("no error: %d %s \n", errno, strerror(errno));
        return -1; 
    }

    if (ftruncate(fd, sizeof(struct region)) == -1) {
        printf("no error: %d %s \n", errno, strerror(errno));
        return -1;
    }

    addr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (addr == MAP_FAILED) {
        printf("no error: %d %s \n", errno, strerror(errno));
        return -1;
    }
        
    strcpy(addr->buf,"");

    for (int i = 0; i < 3; i++) {
        switch (pids[i] = fork()) {
            case -1:
                printf("no error: %d %s \n", errno, strerror(errno));
                return -1;
            case 0:
                if (execl(path, "reader", NULL) == -1) {
                    printf("can not open exec prog \n");
                    return -1;                    
                }
                break;
            default:
                break;
        }
    }

    while(1) {
        sleep(1);
        reserveSem(semid, WRITE_SEM);
        put_randdata();
        releaseSem(semid, READ_SEM);
        for (int i = 0; i < 3; i++)
            if (kill(pids[i], SIGUSR1) == -1) {
                printf("no error: %d %s \n", errno, strerror(errno));
                return -1;
            }
    }
    
    int status;
    for (int i = 0; i < 3; i++)
        if (waitpid(pids[i], &status, 0) == -1) 
            return -1;

    return 0;
}
