#include "shared_resource.h"
#include <errno.h>
#include <sys/ipc.h>
#include <sys/sem.h>

void parentSignal(int sig)
{
    if (sig == SIGUSR1) {
        printf("I recevied the signal\n");
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

int main()
{
    int fd, semid;
    struct region *addr;
    semid = semget(SEM_KEY, 0, 0);
    fd = shm_open("my_region", O_RDWR, S_IRUSR | S_IRWXU );
    if (fd == -1) {
         printf("no error: %d %s \n", errno, strerror(errno));
         return -1;
    }

    addr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED) {
        printf("no error: %d %s \n", errno, strerror(errno));
        return -1;
    }
    
    if (signal(SIGUSR1, parentSignal) == SIG_ERR) 
        printf("no error: %d %s \n", errno, strerror(errno));
    
    for (;;){
        printf("I am the child %d\n", getpid());
        reserveSem(semid, READ_SEM);
        
        if (write(STDOUT_FILENO, addr->buf, strlen(addr->buf)) < 0)
            printf("Chid cant read\n");

        releaseSem(semid, WRITE_SEM);   
        pause(); 
    }  
    return 0;                     
}
