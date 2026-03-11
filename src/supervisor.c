#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "aether_types.h"
#include "utils.h"
#include "ipc.h"
/*
 * Calls fork(). In the child, it triggers the IPC wiring and calls execve().
 * In the parent, it returns the new child's PID.
 */
pid_t supervisor_spawn_worker(aether_worker_t *worker)
{
    pid_t pid_child;
    
    if ((pid_child = Fork()) == 0) {
        //Child
        char *envp[] = {NULL};
        ipc_child_wire_stdout(worker->stdout_pipe);
        Execve(worker->job->command, worker->job->args, envp);
    }
    return pid_child;
}

/*
 * Blocks the parent until the specific worker finishes.
 * Parses the exit status macros and updates worker->state and worker->exit_code.
 */
void supervisor_wait_and_autopsy(aether_worker_t *worker)
{
    worker->state = WORKER_STATE_RUNNING;
    printf("[\033[32mSUPERVISOR\033[0m] Child spawned with PID %d\n", worker->pid);
    fflush(stdout);

    int status;
    // The Wait.
    // waitpid returns the PID of the child that changed state.
    if (waitpid(worker->pid, &status, 0) < 0) {
        unix_err("waitpid error");
    }

    // The Autopsy.
    if (WIFEXITED(status)) {
        // Child exited normally (e.g., via exit() or returning from main)
        int exit_status = WEXITSTATUS(status);
        worker->state = WORKER_STATE_FINISHED_SUCCESS;
        printf("[\033[34mAETHER\033[0m] Child %d exited with status %d\n", worker->pid, exit_status);
        fflush(stdout);
        if (exit_status != 0) {
            printf("[\033[31m!\033[0m] Note: Command returned an error code.\n");
            fflush(stdout);
        }
    } 
    else if (WIFSIGNALED(status)) {
        // Child was terminated by a signal (e.g., Segfault, Kill)
        worker->state = WORKER_STATE_FINISHED_ERROR;
        printf("[\033[31m!\033[0m] Child %d terminated by signal %d\n", 
                worker->pid, WTERMSIG(status));
        fflush(stdout);
    }
}
