#include "shared_resource.h"
#include <errno.h>

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
        // while (received == 0);
        if (pthread_mutex_lock(&mutex) != 0) {
            printf("no error: %d %s \n", errno, strerror(errno));
            return -1;
        }

        if (write(STDOUT_FILENO, addr->buf, strlen(addr->buf)) < 0)
            printf("Chid cant read\n");
        
        if (pthread_mutex_unlock(&mutex) != 0) {
            printf("no error: %d %s \n", errno, strerror(errno));
            return -1;
        }
        pause(); 
    }  
    return 0;                     
}
