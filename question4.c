#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h> 
#include <semaphore.h>

#define MAX_BUFF_ARRAY_LEN 5
#define NUMBERS_LEN 10

static sem_t sem;

int buff_array[MAX_BUFF_ARRAY_LEN] = { 0, 0, 0, 0, 0 };
int curr_buff_index = 0;

typedef struct {
    int *nums;
} producer_args_t;

void display_buffer();
void *producer(void *vptr);
void *consumer(void *vptr);


int main(void) 
{
    sem_init(&sem, 0, 1);
    srand(time(NULL));
    int numbers[NUMBERS_LEN] = { 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0};
    pthread_t pro_tid, con_tid;

    /* prompt user for 10 numbers */
    for (int i = 0;i < NUMBERS_LEN; i++) {
        printf("Enter a number for numbers[%d]: ", i);
        scanf("%d", &numbers[i]);
    }

    /* create thread for both producer and consumer */
    producer_args_t pro_args = { .nums=&numbers[0] };
    pthread_create(&pro_tid, NULL, &producer, (void *) &pro_args);
    pthread_create(&con_tid, NULL, &consumer, NULL);
    pthread_join(pro_tid, NULL);
    pthread_join(con_tid, NULL);

    printf("Content of buffer array after both producer and consumer threads\n");
    for (int i = 0; i < MAX_BUFF_ARRAY_LEN; i++)
    {
        printf("buff_array[%d] = %d\n", i, buff_array[i]);
    }

    sem_destroy(&sem);
    return 0;
}


/* takes array of numbers 
 * add each number to buffer 
 */
void *producer(void *vptr)
{
    producer_args_t *producer_args = (producer_args_t *) vptr;
    int produced = 0;
    while (produced < NUMBERS_LEN) {
        /* sleep for random amount of time */
        int sleep_time = rand() % 10 + 1;
        printf("Producer sleeping for %d seconds\n", sleep_time);
        sleep(sleep_time);
        /* critical section starts */
        // add number from numbers array to buffer
        if (curr_buff_index < MAX_BUFF_ARRAY_LEN)
        {
            sem_wait(&sem);
            // number from numbers array to buffer
            buff_array[curr_buff_index] = producer_args->nums[produced];
            printf("Produced %d\n", buff_array[curr_buff_index]);
            curr_buff_index++;
            produced++;
            display_buffer();
            sem_post(&sem);
        } else {
            /* buffer is full wait for consumer*/
            printf("Buffer is full\n");
            while (curr_buff_index == (MAX_BUFF_ARRAY_LEN - 1));
        }
    }
    pthread_exit(NULL);
}

void display_buffer() {
    printf("current buffer index is %d\n", curr_buff_index);
    for (int i = 0; i < MAX_BUFF_ARRAY_LEN; i++) {
        printf("%d ", buff_array[i]);
    }
    printf("\n");
}

/* consumes number from buffer produced by producer */
void *consumer(void *vptr)
{
    int consumed = 0;
    while (consumed < NUMBERS_LEN) {
        int sleep_time = rand() % 30 + 1;
        printf("Consumer sleeping for %d seconds\n", sleep_time);
        sleep(sleep_time);
        /* consume added numbers from buffer */
        if (curr_buff_index != -1 &&  curr_buff_index >= 0)
        {
            sem_wait(&sem);
            /* critical section starts */
            /* buffer has number in it, consume it */
            printf("Consumed %d\n", buff_array[curr_buff_index-1]);
            buff_array[curr_buff_index-1] = 0;
            curr_buff_index--;
            consumed++;
            display_buffer();
            sem_post(&sem);
        } else {
            /* buffer is empty wait for producer */
            printf("Buffer is empty\n");
            while (curr_buff_index == 0);
        }

    }
    pthread_exit(NULL);
}