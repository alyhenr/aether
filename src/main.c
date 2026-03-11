#include <stdio.h>
#include <stdlib.h>
#include "aether_types.h"
#include "supervisor.h"
#include "ipc.h"
#include "utils.h"

int main(void) {
    printf("[\033[34mAETHER\033[0m] Daemon starting up...\n");

    // 1. Define the Job
    aether_job_t job;
    job.command = "/bin/ls";
    char *args[] = {"ls", "-l", "-a", NULL}; 
    job.args = args;

    // 2. Initialize the Worker Context
    aether_worker_t worker;
    worker.job = &job;
    worker.state = WORKER_STATE_PENDING;

    // 3. Setup the Communication Channels
    if (ipc_init_pipes(worker.stdout_pipe) == -1) {
        perror("Failed to initialize IPC pipes");
        exit(EXIT_FAILURE);
    }

    // 4. Spawn the Worker
    printf("[\033[34mAETHER\033[0m] Spawning worker for command: %s\n", job.command);
    worker.pid = supervisor_spawn_worker(&worker);

    // 5. Wait for the final status
    if (worker.pid > 0) {
        supervisor_wait_and_autopsy(&worker);
    }

    if (worker.pid < 0) {
        exit(EXIT_FAILURE); // Error already printed by supervisor
    }

    // 6. Parent captures output while the child is running
    printf("[\033[34mAETHER\033[0m] Capturing worker output:\n");
    printf("----------------------------------------\n");
    fflush(stdout);
    
    ipc_parent_capture_output(worker.stdout_pipe);
    
    printf("\n----------------------------------------\n");


    printf("[\033[34mAETHER\033[0m] Supervisor shutting down cleanly.\n");
    fflush(stdout);

    return 0;
}
