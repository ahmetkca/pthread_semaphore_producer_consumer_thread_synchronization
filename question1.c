#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <pthread.h> 
#include <semaphore.h>

#define BUFF_LEN 256
#define FROM_MASTER "[Master] "
#define FROM_CHILD1 "\t[Child1] "
#define FROM_CHILD2 "\t[Child2] "

#define CHILD1_TXT_FILENAME "child1.txt"
#define CHILD2_TXT_FILENAME "child2.txt"

void child1()
{
    sleep(1);
    FILE *fd1;
    if ((fd1 = fopen(CHILD1_TXT_FILENAME, "r")) == NULL) {
        fprintf(stderr, FROM_CHILD1 "Error! cannot open file for read\n");
        exit(EXIT_FAILURE);
    }
    char buf[BUFF_LEN];
    fgets(buf, BUFF_LEN, fd1);
    printf(FROM_CHILD1 "%s\n", buf);
    fclose(fd1);
}

void child2()
{
    sleep(1);
    FILE *fd2;
    if ((fd2 = fopen(CHILD2_TXT_FILENAME, "r")) == NULL) {
        fprintf(stderr, FROM_CHILD2 "Error! cannot open file for read\n");
        exit(EXIT_FAILURE);
    }
    char buf[BUFF_LEN];
    fgets(buf, BUFF_LEN, fd2);
    printf(FROM_CHILD2 "%s\n", buf);
    fclose(fd2);
}

int main(void)
{

    int status_cpid1, status_cpid2;
    pid_t cpid1, cpid2;

    /*
     *  Create child process 1 only in the master process
     */
    if ((cpid1 = fork()) == 0) {
        printf(FROM_MASTER "Forked child1\n");
        child1();
    } else if (cpid1 < 0) {
        fprintf(stderr, FROM_MASTER "Error! cannot create a child process\n");
        exit(EXIT_FAILURE);
    }

    /*
     *  Create child process 2 only in the master process
     */
    if ((cpid2 = fork()) == 0 && cpid1 > 0) {
        printf(FROM_MASTER "Forked child2\n");
        child2();
    } else if (cpid2 < 0) {
        fprintf(stderr, FROM_MASTER "Error! cannot create a child process\n");
        exit(EXIT_FAILURE);
    }

    FILE *ch1_fd, *ch2_fd;
    if ((ch1_fd = fopen(CHILD1_TXT_FILENAME, "w")) == NULL) {
        fprintf(stderr, FROM_MASTER "Error! cannot open file\n");
        exit(EXIT_FAILURE);
    }
    if ((ch2_fd = fopen(CHILD2_TXT_FILENAME, "w")) == NULL) {
        fprintf(stderr, FROM_MASTER  "Error! cannot open file\n");
        exit(EXIT_FAILURE);
    }
    fputs("child 1", ch1_fd);
    fputs("child 2", ch2_fd);
    fclose(ch1_fd);
    fclose(ch2_fd);

    waitpid(cpid1, &status_cpid1, 0);
    waitpid(cpid2, &status_cpid2, 0);
    exit(EXIT_SUCCESS);
}