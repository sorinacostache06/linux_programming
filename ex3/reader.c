#include "shared_resource.h"

int received = 0;

void parentSignal(int sig)
{
    if (sig == SIGUSR1) {
        printf("I recevied the signal\n");
        received = 1;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    fd = shm_open("my_region", O_RDWR, S_IRUSR | S_IRWXU );
    if (fd == -1)
        printf("exec error: can open shared memory\n");

    addr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    signal(SIGUSR1, parentSignal);  
    for (;;){
        printf("I am the child %d\n", getpid());
        while (received == 0);
        pthread_mutex_lock(&mutex);

        if (write(STDOUT_FILENO, addr->buf, strlen(addr->buf)) < 0)
            printf("Chid cant read\n");
        
        pthread_mutex_unlock(&mutex);
        pause(); 
    }                       
}
