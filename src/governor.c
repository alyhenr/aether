#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/stat.h>
#include <string.h>

#include "governor.h"

#define CGROUP_BASE "/sys/fs/cgroup/aether"


int governor_enforce_limits(pid_t pid, uint32_t mem_limit_mb)
{
    char cgroup_path[256];
    // char file_path[256];

    // 1. Ensure the base Aether cgroup exists.
    // In a real production system, this is created by systemd on boot.
    // For the daemon (maybe - TO DEFINE), it attempt to create it (ignoring errors if it already exists).
    mkdir(CGROUP_BASE, 0755);

    // 2. Create the specific directory for this worker (e.g., /sys/fs/cgroup/aether/job_1234)
    snprintf(cgroup_path, sizeof(cgroup_path), "%s/job_%d", CGROUP_BASE, pid);
    
    if (mkdir(cgroup_path, 0755) == -1) {
        perror("[GOVERNOR] Failed to create worker cgroup");
        // Note: If you don't run `sudo make run`, this will fail with Permission Denied.
        return -1; 
    }

    // TODO 1: Limit Memory
    // Calculate the memory limit in bytes (mem_limit_mb * 1024 * 1024).
    // Construct the path to the "memory.max" file inside cgroup_path.
    // Open the file, write the bytes as a string, and close it.
    
    // TODO 2: Disable Swap (Crucial for FaaS)
    // If you limit RAM but leave Swap enabled, the process will just thrash the hard drive.
    // Construct the path to "memory.swap.max" inside cgroup_path.
    // Open it, write "0", and close it.

    // TODO 3: Move the Process into the Cgroup
    // Construct the path to "cgroup.procs" inside cgroup_path.
    // Open it, write the 'pid' integer as a string, and close it.

    printf("[\033[35mGOVERNOR\033[0m] Limits enforced for PID %d (%d MB)\n", pid, mem_limit_mb);
    return 0;

    
    return 0;
}
