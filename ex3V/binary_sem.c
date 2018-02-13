#include "binary_sem.h"
#include <sys/sem.h>
#include <sys/types.h>
#include <errno.h>

union semun {
            int val;
            struct semid_ds *buf;
            unsigned short  *array;
};

int bsUseSemUndo = 0;
int bsRetryOnEintr = 1;

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

int semDeallocate (int semid)
{
    union semun ignored_argument;
    return semctl (semid, 1, IPC_RMID, ignored_argument);
}
