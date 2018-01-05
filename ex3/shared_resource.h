#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define FLAG O_CREAT

sem_t *sem;

struct region {
    int cnt;
    char buf[BUF_SIZE];
};

struct region *addr;
