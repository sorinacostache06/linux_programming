#include "shared_resource.h"
#include <string.h>
#include <stdlib.h>
#include <time.h>

void put_randdata()
{
    int r;
    srand(time(NULL));
    // strcpy(addr->buf,"");

    for (int i = 0; i < 5; i++) {
        r = rand() % 1000 + 1;
        char c[3];
        sprintf(c, "%d", r);
        strcat(addr->buf, c);
    }
}

int main(int argc, char *argv[])
{
    int r, fd;
    int done = 1; 
    pid_t pid;  
    fd = shm_open("my_region", O_CREAT | O_RDWR, S_IRUSR | S_IRWXU );
    
    if (fd == -1) 
        printf("error create shared memory\n");

    if (ftruncate(fd, sizeof(struct region)) == -1)
        printf("error truncate\n");

    addr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    if (addr == MAP_FAILED)
        printf("error: map failed\n");

    printf("%s\n", addr->buf);

    for (int i = 0; i < 3; i++) {
        switch (pid = fork()) {
            case -1:
                printf("error: fork %d exit\n", i);
                return -1;
            case 0:
                printf("Child %d\n", getpid());
                if (execl("/home/sacostache/Documents/work/linux-programming/ex3/reader", "reader", NULL) == -1)
                    printf("can not open exec prog \n");
                for(;;);
                break;
            default:
                // while (done) {
                    sleep(1);
                    put_randdata();
                    kill(pid, SIGUSR1);
                // }
                break;
        }
    }
}