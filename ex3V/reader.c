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
