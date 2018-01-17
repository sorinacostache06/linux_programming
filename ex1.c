#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>
#include <sys/wait.h>

#define GEN_TIME 100
#define MAX_BUFF 10

int counter, r;
int buffer[MAX_BUFF];
sem_t full, empty;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t prod, cons;

int isprime(int n)
{
    sleep(5);
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

int insert_item(int item)
{
    sem_wait(&empty);
    pthread_mutex_lock(&mutex);

    if (counter < MAX_BUFF) {
        buffer[counter++] = item;
        return 0;
    } else {
        while( counter < MAX_BUFF)
            wait(NULL);
        return 0;
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&full);
    return -1;
}

int remove_item(int item)
{
    sem_wait(&full);
    pthread_mutex_lock(&mutex);
    if (counter > 0) {
        item = buffer[0];
        counter--;
        for (int i = 0; i < counter; i++)
            buffer[i] = buffer[i+1];
        return 0;
    }
    pthread_mutex_unlock(&mutex);
    sem_post(&empty);
    return -1;
}

void *producer(void *arg)
{
    while(1) {    
    // for (int i = 0; i < 4; i++) {
        srand(time(NULL));
        r = rand() % 1000 + 100;
        insert_item(r);
        for (int i = 0; i < counter; i++)
            printf("%d ", buffer[i]);
        printf("\n");
        usleep(GEN_TIME);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{        
    int item;
    while(1) {
    // for (int i = 0; i < 4; i++) {
        while (counter == 0)
            wait(NULL);

        item = buffer[0];
        remove_item(item);

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

    pthread_mutex_init(&mutex, NULL);
    sem_init(&empty, 0,100); // All of buffer is empty
    sem_init(&full, 0, 0);
    // if (sem_init(&prod, 0, 0) == -1)
    //     printf("error init semaphore");
    // if (sem_init(&cons, 0, 1) == -1)
    //     printf("error init semaphore");

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

    pthread_mutex_destroy(&mutex);
    return 0;
}
