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

#define BUF_SIZE 100000
#define FLAG O_CREAT

struct region {
    int cnt;
    char buf[BUF_SIZE];
};

struct region *addr;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

