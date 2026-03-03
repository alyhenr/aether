#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>      // For fork(), execve(), sleep()
#include <sys/wait.h>    // For waitpid() and macros
#include <sys/types.h>   // For pid_t

#include "aether_types.h"
#include "utils.h"

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
    worker.pid = Fork(); //Error is handled inside the custom Fork

    if (worker.pid == 0) {
        /* ========================================================
         * CHILD PROCESS REALM
         * ========================================================
         * If fork() returned 0, we are the child. 
         * We need to abandon the Aether code and become the user's job.
         */
        
        /// TODO 1: Close the READ end of the pipe in the child. The child only writes.
        
        // TODO 2: Use dup2() to duplicate the WRITE end of the pipe onto STDOUT_FILENO (which is 1).
        
        // TODO 3: Close the original WRITE end of the pipe, since dup2 made a copy of it on FD 1.
        
        // Now execute the binary. Since STDOUT is now pointing to the pipe, 
        // the 'ls' command will unknowingly write its output straight into our kernel buffer.

        printf("[\033[32mWORKER\033[0m] Child worker executing binary...\n");
        char *envp[] = {NULL};
        Execve(job.command, job.args, envp);

    } 
    else {
        /* ========================================================
         * PARENT PROCESS REALM (THE SUPERVISOR)
         * ========================================================
         * If fork() returned > 0, we are the parent, and the number is the child's PID.
         */
        worker.state = WORKER_STATE_RUNNING;
        printf("[\033[32mSUPERVISOR\033[0m] Child spawned with PID %d\n", worker.pid);

        // TODO 4: Close the WRITE end of the pipe in the parent. The parent only reads.
        
        // --- NEW: Read from the pipe ---
        char buffer[1024];
        ssize_t bytes_read;
        
        printf("[\033[34mAETHER\033[0m] Capturing worker output:\n");
        printf("----------------------------------------\n");
        
        // TODO 5: Write a while loop that uses read() to pull data from the READ end 
        // of the pipe into 'buffer' until read() returns 0 (End of File).
        // Inside the loop, use write(STDOUT_FILENO, buffer, bytes_read) to print it to your screen.
        
        printf("\n----------------------------------------\n");

        int status;
        // The Wait.
        // waitpid returns the PID of the child that changed state.
        if (waitpid(worker.pid, &status, 0) < 0) {
            unix_err("waitpid error");
        }

        // The Autopsy.
        if (WIFEXITED(status)) {
            // Child exited normally (e.g., via exit() or returning from main)
            int exit_status = WEXITSTATUS(status);
            worker.state = WORKER_STATE_FINISHED_SUCCESS;
            printf("[\033[34mAETHER\033[0m] Child %d exited with status %d\n", worker.pid, exit_status);
            
            if (exit_status != 0) {
                printf("[\033[31m!\033[0m] Note: Command returned an error code.\n");
            }
        } 
        else if (WIFSIGNALED(status)) {
            // Child was terminated by a signal (e.g., Segfault, Kill)
            worker.state = WORKER_STATE_FINISHED_ERROR;
            printf("[\033[31m!\033[0m] Child %d terminated by signal %d\n", 
                   worker.pid, WTERMSIG(status));
        }
        
        printf("[\033[34mAETHER\033[0m] Supervisor shutting down cleanly.\n");
    }

    return 0;
}
