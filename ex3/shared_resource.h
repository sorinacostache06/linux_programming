#include <stdio.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <signal.h>
#include <sys/types.h>
#include <unistd.h>
#include <pthread.h>
#include <stdlib.h>
#include <string.h>
#include <semaphore.h>

#define BUF_SIZE 100000

struct region {
    int cnt;
    char buf[BUF_SIZE];
};

extern sem_t sem;