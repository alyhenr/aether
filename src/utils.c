#include <unistd.h>
#include <errno.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>

extern char **environ; 

void unix_err(char *msg) 
{
    fprintf(stderr, "%s: %s\n", msg, strerror(errno));
    exit(0);
}

int Fork(void) 
{
    pid_t pid;

    if ((pid = fork()) < 0) {
        unix_err("fork error");
    }

    return pid;
}

void Execve(const char *pathname, char *const argv[], char *const envp[])
{
    if (envp[0] != NULL) {
        printf("OK");
    }
    printf("Calling execve for binary at: %s\n", pathname);
    fflush(stdout);
    
    if (execve(pathname, argv, environ) < 0) {
        fprintf(stderr, "execve failed: %s\n", strerror(errno));
        exit(1);
    }
}
