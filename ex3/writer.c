#include "shared_resource.h"

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
    int r, fd;
    pid_t pid;

    fd = shm_open("my_region", O_CREAT | O_RDWR, S_IRUSR | S_IRWXU );
    
    if (fd == -1) 
        printf("error create shared memory\n");

    if (ftruncate(fd, sizeof(struct region)) == -1)
        printf("error truncate\n");

    addr = mmap(NULL, sizeof(struct region), PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    close(fd);

    if (addr == MAP_FAILED)
        printf("error: map failed\n");
        
    strcpy(addr->buf,"");

    for (int i = 0; i < 3; i++) {
        switch (pid = fork()) {
            case -1:
                printf("error: fork %d exit\n", i);
                return -1;
            case 0:
                if (execl("/home/sacostache/Documents/work/linux-programming/ex3/reader", "reader", NULL) == -1)
                    printf("can not open exec prog \n");
                break;
            default:
                for (int i = 0; i < 10; i++) {
                    sleep(1);
                    pthread_mutex_lock(&mutex);
                    put_randdata();
                    pthread_mutex_unlock(&mutex);
                    kill(pid, SIGUSR1);
                }
                break;
        }
    }
}
