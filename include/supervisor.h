#ifndef SUPERVISOR_H
#define SUPERVISOR_H

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
pid_t supervisor_spawn_worker(aether_worker_t *worker);

/*
 * Blocks the parent until the specific worker finishes.
 * Parses the exit status macros and updates worker->state and worker->exit_code.
 */
void supervisor_wait_and_autopsy(aether_worker_t *worker);

#endif // SUPERVISOR_H
