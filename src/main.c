#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // For fork(), execve(), sleep()
#include <sys/wait.h>    // For waitpid() and macros
#include <sys/types.h>   // For pid_t
#include "aether_types.h"

int main(void) {
    printf("[\033[34mAETHER\033[0m] Daemon starting up...\n");

    // 1. Mock a Job Request
    // In the future, this comes from a TCP socket. Today, we hardcode it.
    aether_job_t job;
    job.command = "/bin/ls"; // Let's just run 'ls' to list directory contents
    
    // execve() requires a null-terminated array of strings.
    // The first argument is conventionally the name of the program itself.
    char *args[] = {"ls", "-l", "-a", NULL}; 
    job.args = args;

    // 2. Initialize the Worker Context
    aether_worker_t worker;
    worker.job = &job;
    worker.state = WORKER_STATE_PENDING;

    printf("[\033[34mAETHER\033[0m] Spawning worker for command: %s\n", job.command);

    // 3. The Great Schism
    worker.pid = fork();

    if (worker.pid < 0) {
        // Fork failed (e.g., system is out of memory or hit process limits)
        perror("fork failed");
        exit(EXIT_FAILURE);
    } 
    else if (worker.pid == 0) {
        /* ========================================================
         * CHILD PROCESS REALM
         * ========================================================
         * If fork() returned 0, we are the child. 
         * We need to abandon the Aether code and become the user's job.
         */
        
        // TODO 1: Call execve() using worker.job->command and worker.job->args.
        // Note: You will need to pass an environment array too. You can use a generic 
        // empty environment for now: char *envp[] = {NULL};
        
        // TODO 2: Handle the error. 
        // If execve() succeeds, it NEVER returns. If it returns, something went wrong 
        // (e.g., the binary doesn't exist). You must print an error and exit() immediately.

    } 
    else {
        /* ========================================================
         * PARENT PROCESS REALM (THE SUPERVISOR)
         * ========================================================
         * If fork() returned > 0, we are the parent, and the number is the child's PID.
         */
        worker.state = WORKER_STATE_RUNNING;
        printf("[\033[32mSUPERVISOR\033[0m] Child spawned with PID %d\n", worker.pid);

        // TODO 3: The Wait.
        // Use waitpid() to pause the parent until the specific worker.pid finishes.
        // You need to pass a reference to an integer to store the 'status'.
        
        // TODO 4: The Autopsy.
        // Once waitpid() returns, use the WIFEXITED() and WEXITSTATUS() macros 
        // on the status integer to figure out if the child succeeded or failed.
        // Print the result to the console and update worker.state.
        
        printf("[\033[34mAETHER\033[0m] Supervisor shutting down cleanly.\n");
    }

    return 0;
}
