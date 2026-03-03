#ifndef AETHER_TYPES_H
#define AETHER_TYPES_H

#include <stdint.h>
#include <sys/types.h>
#include <time.h>

/* * The State Machine for our Worker Process. 
 * Aether needs to know exactly what phase of life the process is in.
 */
typedef enum {
    WORKER_STATE_PENDING = 0,      // Created in memory, but fork() hasn't been called
    WORKER_STATE_RUNNING,          // Currently executing the user's code
    WORKER_STATE_FINISHED_SUCCESS, // Exited normally (Code 0)
    WORKER_STATE_FINISHED_ERROR,   // Exited with error (Code != 0)
    WORKER_STATE_OOM_KILLED,       // Killed by the kernel (Cgroups limit reached)
    WORKER_STATE_TIMEOUT_KILLED    // Killed by Aether (Took too long)
} worker_state_t;

/*
 * The Job Definition.
 * This represents the "Request" before it becomes a real OS process.
 * It contains the instructions and the constraints.
 */
typedef struct {
    char *command;          // The binary to execute (e.g., "/bin/untrusted_app")
    char **args;            // Null-terminated array of arguments for execve()
    uint32_t mem_limit_mb;  // Maximum RAM allowed in Megabytes
    uint32_t cpu_limit_pct; // Maximum CPU usage allowed (e.g., 50%)
    uint32_t timeout_ms;    // Maximum time allowed before assassination
} aether_job_t;

/*
 * The Worker Context.
 * This is the beating heart of the Supervisor. It tracks the actual
 * Linux process, its file descriptors, and its telemetry.
 */
typedef struct {
    pid_t pid;                 // The OS Process ID (0 if not spawned yet)
    worker_state_t state;      // Current lifecycle state
    
    // File Descriptors for Inter-Process Communication (IPC)
    // fd[0] is for reading, fd[1] is for writing.
    int stdout_pipe[2];        // Captures what the child printf's
    int stderr_pipe[2];        // Captures the child's errors
    
    // Telemetry & Timing
    struct timespec start_time;
    struct timespec end_time;
    
    // The result
    int exit_code;             // Populated via waitpid() macros (WEXITSTATUS / WTERMSIG)
    
    // Link back to the rules that govern this worker
    aether_job_t *job;         
} aether_worker_t;

#endif // AETHER_TYPES_H
