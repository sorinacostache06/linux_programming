#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include <stdlib.h>
#include <unistd.h>
#include <time.h>

#define GEN_TIME 1000000

int r = 0;
pthread_cond_t condc = PTHREAD_COND_INITIALIZER;
pthread_cond_t condp = PTHREAD_COND_INITIALIZER;
pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

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

void *producer(void *arg)
{
    for (int i = 0; i <= 3; i++) {
        pthread_mutex_lock(&mutex);
        while (r != 0) 
            pthread_cond_wait(&condp, &mutex);

        srand(time(NULL));
        r = rand() % 1000 + 100;
        pthread_cond_signal(&condc);
        pthread_mutex_unlock(&mutex);
        usleep(GEN_TIME);
    }
    pthread_exit(0);
}

void *consumer(void *arg)
{
    for (int i = 0; i <= 3; i++) {
        pthread_mutex_lock(&mutex);

        while (r == 0)
            pthread_cond_wait(&condc, &mutex);

        if (isprime(r) == -1)
            printf("Thread <%ld>: number %d, prime: [NO]\n", pthread_self(), r);
        else
            printf("Thread <%ld>: number %d, prime: [YES]\n", pthread_self(), r);

        r = 0;
        pthread_cond_signal(&condp);
        pthread_mutex_unlock(&mutex);

    }
    pthread_exit(0);
}

int main()
{
    pthread_t c, p;
    int s, s1;

    s1 = pthread_create(&p, NULL, producer, NULL);
    if (s1 != 0) 
        printf("error: threand was not created");

    s = pthread_create(&c, NULL, consumer, NULL);
    if (s != 0) 
        printf("error: threand was not created");

    s = pthread_join(p, NULL);
    s1 = pthread_join(c, NULL);

    pthread_mutex_destroy(&mutex);
    pthread_cond_destroy(&condp);
    pthread_cond_destroy(&condc);
    return 0;
}
