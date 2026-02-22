#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
void unix_err(char *msg) 
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

int Fork() 
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        unix_err("fork error");
    } else if (pid > 0) {
        printf("Parent: my PID = %d, child PID = %d\n",
               getpid(), pid);
    } else {
        printf("Child: my PID = %d, parent PID = %d\n",
               getpid(), getppid());
    }

    return pid;
}