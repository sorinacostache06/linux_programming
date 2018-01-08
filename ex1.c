#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>
#include <semaphore.h>

#define GEN_TIME 1000000
#define MAX_BUFF 1000

int counter, r;
int buffer[MAX_BUFF];

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

sem_t prod, cons;

int isprime(int n)
{
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
    if (counter < MAX_BUFF) {
        buffer[counter++] = item;
        return 0;
    }
    return -1;
}

int remove_item(int *item)
{
    if (counter > 0) {
        *item = buffer[counter - 1];
        counter--;
        return 0;
    }
    return -1;
}

void *producer(void *arg)
{
    for (int i = 0; i <= 10; i++) {
        sem_wait(&cons);
        pthread_mutex_lock(&mutex);
        
        srand(time(NULL));
        r = rand() % 1000 + 100;
        
        pthread_mutex_unlock(&mutex);
        sem_post(&prod);
        sleep(GEN_TIME);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{
    for (int i = 0; i <= 10; i++) {
        sem_wait(&prod);
        pthread_mutex_lock(&mutex);

        if (isprime(r) == -1)
            printf("Thread <%ld>: number %d, prime: [NO]\n", pthread_self(), r);
        else
            printf("Thread <%ld>: number %d, prime: [YES]\n", pthread_self(), r);

        pthread_mutex_unlock(&mutex);
        sem_post(&cons);
    }
    pthread_exit(0);
}

int main()
{
    pthread_t p, c;
    int s, s1;

    if (sem_init(&prod, 0, 0) == -1)
        printf("error init semaphore");
    if (sem_init(&cons, 0, 1) == -1)
        printf("error init semaphore");

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
