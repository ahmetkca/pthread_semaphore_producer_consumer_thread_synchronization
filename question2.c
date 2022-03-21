#define _XOPEN_SOURCE 700 // required for barriers to work
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <errno.h>
#include <pthread.h> 
#include <semaphore.h>

int main(void)
{
    int status, returned_pid;
    pid_t cpid;
    if ((cpid = fork()) > 0) {
        // parent
        while (returned_pid = wait(&status)) {   /* go to sleep until something happens */
            /* wait woke up */
            if (returned_pid == cpid)
                break;	/* this is what we were looking for */
            if ((returned_pid == -1) && (errno != EINTR)) {
                /* an error other than an interrupted system call */
                perror("waitpid");
                return 1;
            }
        }
        printf("Parent process\n");
        if (WIFEXITED(status))	/* process exited normally */
            printf("child process exited with value %d\n", WEXITSTATUS(status));
        else if (WIFSIGNALED(status))	/* child exited on a signal */
            printf("child process exited due to signal %d\n", WTERMSIG(status));
        else if (WIFSTOPPED(status))	/* child was stopped */
            printf("child process was stopped by signal %d\n", WIFSTOPPED(status));
    } else if (cpid == 0) {
        // child
        sleep(1);
        printf("Child process\n");
    } else {
        fprintf(stderr,"Error! cannot fork a child process\n");
        exit(EXIT_FAILURE);
    }
    return 0;
}