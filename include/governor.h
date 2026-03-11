#ifndef GOVERNOR_H
#define GOVERNOR_H

#include <sys/types.h>
#include <stdint.h>

/*
 * Creates a cgroup for the given PID and enforces the memory limit.
 * Must be called by the CHILD process before calling execve().
 * Returns 0 on success, -1 on failure.
 */
int governor_enforce_limits(pid_t pid, uint32_t mem_limit_mb);

#endif
