#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/wait.h>
#include <errno.h>

#define GEN_TIME 100
#define MAX_BUFF 10

int counter, r;
int buffer[MAX_BUFF];
sem_t full, empty;

int isprime(int n)
{
    sleep(10);
    int i = 3;
    if (r < 2) 
        return -1;
    if (r == 2) 
        return 0;
    if (r % 2 == 0) 
        return -1;
    while (i * i <= r) {
        if (r % i == 0) 
            return -1;
        i += 2;
    }
    return 0;
}

void insert_item(int item)
{
    sem_wait(&empty);
    if (counter < MAX_BUFF) {
        buffer[counter++] = item;
    } else {
        while (counter < MAX_BUFF)
            wait(NULL);
    }
    for (int i = 0; i < counter; i++)
            printf("%d ", buffer[i]);
    printf("\n");
    sem_post(&full);
}

int remove_item()
{
    sem_wait(&full);
    int item = buffer[0];
    if (counter > 0) {
        counter--;
        for (int i = 0; i < counter; i++)
            buffer[i] = buffer[i+1];
    } else {
        while (counter < 0)
            wait(NULL);
    }
    sem_post(&empty);
    return item;
}

void *producer(void *arg)
{
    while(1) {    
        srand(time(NULL));
        r = rand() % 1000 + 100;
        insert_item(r);
        usleep(GEN_TIME);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{        
    int item;
    while(1) {
        while (counter == 0)
            wait(NULL);

        item = remove_item();

        if (isprime(item) == -1)
            printf("Thread <%ld>: number %d, prime: [NO]\n", pthread_self(), item);
        else
            printf("Thread <%ld>: number %d, prime: [YES]\n", pthread_self(), item);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t p, c;
    int s, s1;
    if (sem_init(&empty, 0, MAX_BUFF) == -1) {
        printf("Semaphore init no error: %d %s \n", errno, strerror(errno));
        return -1;
    }
    if (sem_init(&full, 0, 0) == -1) {
        printf("Semaphore init no error: %d %s \n", errno, strerror(errno));
        return -1;
    }
    s = pthread_create(&p, NULL, producer, NULL);
    if (s != 0) 
        printf("error: threand was not created");

    for (int i = 0; i < 3; i++) {
        s1 = pthread_create(&c, NULL, consumer, NULL);
        if (s1 != 0) 
            printf("error: threand was not created");
    }
        
    s = pthread_join(p, NULL);
    for (int i = 0; i < 3; i++)
        s1 = pthread_join(c, NULL);

    if (sem_destroy(&empty) == -1) {
        printf("Semaphore destroy no error: %d %s \n", errno, strerror(errno));
        return -1;
    }

    if (sem_destroy(&full) == -1) {
        printf("Semaphore destroy no error: %d %s \n", errno, strerror(errno));
        return -1;
    }
    return 0;
}
