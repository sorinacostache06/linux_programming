#include <stdio.h>
#include <semaphore.h>
#include <errno.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>

#define THINKING 0
#define EATING 1
#define HUNGRY 2
#define N 5
#define LEFT (id + 4) % N
#define RIGHT (id + 1) % N


int state[N];
int philos[N] = {0, 1, 2, 3, 4};

sem_t control_states;
sem_t control_philo[N];

int test(int id)
{
    if(state[id] == HUNGRY && state[LEFT] != EATING && state[RIGHT] != EATING) {
        state[id] = EATING;
        printf("Philosopher %d takes fork from %d and %d\n", id, LEFT, RIGHT);
        printf("Philosopher %d is eating!\n", id);
        sem_post(&control_philo[id]);
    }
}

int take_fork(int id)
{
    if (sem_wait(&control_states) == -1) {
        printf("error sem_wait: %s\n", strerror(errno));
        return -1;
    }
    state[id] = HUNGRY;
    printf("Philosopher %d is hungry!\n", id);
    test(id);
    if (sem_post(&control_states) == -1) {
        printf("error sem_post: %s\n", strerror(errno));
        return -1;
    }
    sem_wait(&control_philo[id]);
}

int put_fork(int id)
{
    if (sem_wait(&control_states) == -1) {
        printf("error sem_wait: %s\n", strerror(errno));
        return -1;
    }
    state[id] = THINKING;
    printf("Philosopher %d is thinking!\n", id);
    test(LEFT);
    test(RIGHT);
    if (sem_post(&control_states) == -1) {
        printf("error sem_post: %s\n", strerror(errno));
        return -1;
    }
}

void *philosopher(void *arg)
{
    while(1) {
        int id = *((int *) arg);
        take_fork(id);
        sleep(4);
        put_fork(id);
    }
}

int main(int argc, char *argv[])
{
    pthread_t thread_id[N];
    int s;
    if (sem_init(&control_states, 0, 1) == -1) {
        printf("error sem_init control states: %s\n", strerror(errno));
        return -1;
    }

    for (int i = 0; i < N; i++) 
        if (sem_init(&control_philo[i], 0, 0) == -1) {
            printf("error sem init control philo: %s\n", strerror(errno));
            return -1;
        }
    
    for (int i = 0; i < N; i++) {
        s = pthread_create(&thread_id[i], NULL, philosopher, &philos[i]);
        if (s != 0) {
            printf("error pthread create: %s\n", strerror(errno));
            return -1;
        }
        printf("Thread %i was created!\n", i);
    }
    
    for (int i = 0; i < N; i++) {
        s = pthread_join(thread_id[i], NULL);
        if (s != 0) {
            printf("error thread join %s\n", strerror(errno));
        }
    }
}
