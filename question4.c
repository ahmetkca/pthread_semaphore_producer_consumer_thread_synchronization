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

static int *buff_array;
static int curr_size = 0;



typedef struct {
    int *nums;
} producer_args_t;

void add_to_buffer(int num);
int remove_last_from_buffer();

void *producer(void *vptr);
void *consumer(void *vptr);


int main(void) 
{
    add_to_buffer(10);
    add_to_buffer(20);
    printf("%d\n", buff_array[0]);
    printf("%d\n", buff_array[1]);
    int rem = remove_last_from_buffer();
    printf("%d\n", rem);
    return 0;
    srand(time(NULL));
    int numbers[NUMBERS_LEN] = { 0, 0, 0, 0, 0,
                                    0, 0, 0, 0, 0};
    pthread_t pro_tid, con_tid;
    for (int i = 0;i < NUMBERS_LEN; i++) {
        printf("Enter a number for numbers[%d]: ", i);
        scanf("%d", &numbers[i]);
    }
    producer_args_t pro_args = { .nums=&numbers[0] };
    pthread_create(&pro_tid, NULL, &producer, (void *) &pro_args);
    pthread_join(pro_tid, NULL);
    return 0;
}

void add_to_buffer(int num)
{
    if (buff_array == NULL && curr_size == 0) {
        buff_array = (int *) malloc(sizeof(int));
        buff_array[0] = num;
        curr_size++;
    }
    else if (curr_size <= MAX_BUFF_ARRAY_LEN) {
        buff_array = (int *) realloc(buff_array, (curr_size + 1) * sizeof(int));
        buff_array[curr_size] = num;
        curr_size++;
    }
}

int remove_last_from_buffer()
{
    if (curr_size > 0) {
        int n = buff_array[curr_size-1];
        free(buff_array[curr_size-1]);
        curr_size--;
        return n;
    }
    return -1;
}

void *producer(void *vptr)
{
    producer_args_t *producer_args = (producer_args_t *) vptr;

    for (int i = 0; i < NUMBERS_LEN; i++)
    {
        sleep(rand() % 10 + 1);

    }
    pthread_exit(NULL);
}

void *consumer(void *vptr)
{
    for (int i = 0; i < NUMBERS_LEN; i++)
    {
        sleep(rand() % 10 + 1);

    }
    pthread_exit(NULL);
}