#include "shared_resource.h"
#include <errno.h>
#include <wait.h>

#define MAX_PID 100

int pids[100];
sem_t sem;
struct region *addr;

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

int main(int argc, char *argv[])
{
    int fd;
    char *path;

    if (sem_init(&sem, 1, 1) == -1) {
        printf("Semaphore init no error: %d %s \n", errno, strerror(errno));
        return -1; 
    }

    if (argc < 2){
        printf("error: no arguments\n");
        return -1;
    } else if (argc > 2) {
        printf("error: too many arguments\n");
        return -1;
    }

    path = argv[1];

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
                if (execl(path, "reader", NULL) == -1)
                    printf("can not open exec prog \n");
                break;
            default:
                break;
        }
    }

    while(1) {
        sleep(1);
        if (sem_wait(&sem) == -1) {
            printf("no error: %d %s \n", errno, strerror(errno));
            return -1;
        }
        put_randdata();
        if (sem_post(&sem) == -1) {
            printf("no error: %d %s \n", errno, strerror(errno));
            return -1;
        }

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
