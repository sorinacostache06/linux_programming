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
#include "binary_sem.h"

#define BUF_SIZE 100000
#define SEM_KEY 0x5678
#define WRITE_SEM 0
#define READ_SEM 1
#define OBJ_PERMS (S_IRUSR | S_IWUSR)

struct region {
    int cnt;
    char buf[BUF_SIZE];
};
