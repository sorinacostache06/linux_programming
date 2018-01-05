#include <stdio.h>
#include <stdlib.h>
#include "shared_resource.h"


int received = 0;

void parentSignal(int sig)
{
    if (sig == SIGUSR1) {
        printf("I recevied the signal");
        received = 1;
    }
}

int main(int argc, char *argv[])
{
    int fd;
    fd = shm_open("my_region", O_RDWR, S_IRUSR | S_IRWXU );

    if (fd == -1)
        printf("exec error: can open shared memory\n");

    signal(SIGUSR1, parentSignal);  

    printf("I am the child\n");
    while (received == 0);

    // if (read(STDOUT_FILENO, addr->buf, BUF_SIZE) < 0)
    //         printf("exec error: can not write in stdout!\n");
    // else 
    // printf("Chid read\n");

     _exit(EXIT_SUCCESS);      
}