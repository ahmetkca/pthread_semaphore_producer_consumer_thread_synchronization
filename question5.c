#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <pthread.h>
#include <semaphore.h>

#define NUMBERS_FILENAME "numbers.txt"
#define SUM_FILENAME "sum.txt"
#define NUM_NUMBERS 5

static sem_t sem;

int total_sum = 0;

void *factorial(void *vptr);
int child_process();

int main(void)
{
    /* prompt user for 5 numbers then write to file called numbers.txt */
    FILE *fd = fopen(NUMBERS_FILENAME, "w");
    for (int i = 0; i < NUM_NUMBERS; i++)
    {
        printf("Enter number #%d: ", i);
        int num;
        scanf("%d", &num);
        fprintf(fd, "%d\n", num);
    }
    fclose(fd);

    sem_init(&sem, 1, 1);

    int status;
    pid_t cpid;
    if ((cpid = fork()) == 0) {
        // child
        child_process();
    } else if (cpid > 0) {
        // parent

        /* wait for child process to create 5 threads */
        printf("Waiting for child process...\n");
        waitpid(cpid, &status, 0);
        printf("Child has been waited successfully\n");

        /* read total summation of given number's factorials from file called sum.txt*/
        FILE *sum_fd = fopen(SUM_FILENAME, "r");
        int total_sum_from_file;
        fscanf(sum_fd, "%d", &total_sum_from_file);
        printf("total sum = %d\n", total_sum_from_file);
    } else {
        // fork error
        printf("Error: fork failed\n");
        return 1;
    }
    sem_destroy(&sem);
    return 0;
}

/* coroutine for calculating factorial of given number then adding the calculated factorial to total sum global variable */
void *factorial(void *vptr) {
    int *num = (int *) vptr;
    int num_val = *num;
    int fac = 1;
    for (int i = 2; i <= num_val; i++)
    {
        fac *= i;
    }
    sem_wait(&sem);
    total_sum += fac;
    printf("total_sum = %d, factorial of %d = %d\n", total_sum, num_val, fac);
    sem_post(&sem);

    pthread_exit(NULL);
}

/* child process 
 * responsible for reading numbers from numbers.txt
 * creating thread for each read number
 * wait for the threads to finish
 * then write the value of global variable total sum to file called sum.txt
 */
int child_process() {
    pthread_t threads[NUM_NUMBERS];
    FILE *fd = fopen(NUMBERS_FILENAME, "r");
    int nums[NUM_NUMBERS];

    /* read file numbers.txt and store each number in nums array */
    for (int i = 0; i < NUM_NUMBERS; i++)
    {
        if (fscanf(fd, "%d", &nums[i]) != 1) {
            printf("Error: read failed\n");
            return 1;
        }
    }

    /* create coroutine for each stored number */
    for (int i = 0; i < NUM_NUMBERS; i++)
    {
        printf("num %d\n", nums[i]);
        pthread_create(&threads[i], NULL, &factorial, (void *) &nums[i]);
    }
    fclose(fd);

    /* wait for threads to finish in the child process */
    for (int a = 0; a < NUM_NUMBERS; a++)
    {
        pthread_join(threads[a], NULL);
    }

    /* write the global variable total_sum to sum.txt */
    FILE *sum_fd = fopen(SUM_FILENAME, "w");
    fprintf(sum_fd, "%d", total_sum);
    fclose(sum_fd);
    return 0;
}