#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h> 
#include <semaphore.h>

#define ARRAY_LEN 5

int moving_sum[ARRAY_LEN];
sem_t sem;

typedef struct params {
    int number;
    int index;
} params_t;

void *factorial(void *vptr)
{
    params_t *params = (params_t *) vptr;
    int fac = 1;
    for (int i = 2; i <= params->number; i++) {
        fac *= i;
    }
    int prev = 0;
    if (params->index > 0) {
        while (moving_sum[params->index-1] <= 0)
            sem_wait(&sem);
        /*
        *  critical section starts
         */
        prev = moving_sum[params->index-1];
        moving_sum[params->index] = fac + prev;
        sem_post(&sem);
    } else {
        moving_sum[0] = fac;
    }
    pthread_exit(NULL);
}

int main(void)
{
    for (int i = 0; i < ARRAY_LEN; i++) {
        moving_sum[i] = 0;
    }

    sem_init(&sem, 0, 1);
    pthread_t threads[ARRAY_LEN];
    for (int i = 0; i < ARRAY_LEN; i++) {
        int num;
        printf("Enter number #%d: ", i);
        scanf("%d", &num);
        params_t params = { .number=num, .index=i };
        pthread_create(&threads[i], NULL, &factorial, (void *) &params);
    }

    for (int i = 0; i < ARRAY_LEN; i++) {
        pthread_join(threads[i], NULL);
    }

    for (int i = 0; i < ARRAY_LEN; i++) {
        printf("moving_sum[%d] = %d\n", i, moving_sum[i]);
    }

    sem_destroy(&sem);
    return 0;
}