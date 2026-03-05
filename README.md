# Aether: Bare-Metal FaaS Runtime Engine

(ON-PROGRESS)

Aether is a high-performance, single-node Function-as-a-Service (FaaS) daemon written in C11. 

Rather than relying on high-level container runtimes (like Docker or containerd), Aether interfaces directly with the Linux kernel to provision ephemeral, secure, and resource-constrained execution environments in milliseconds. It is designed to safely execute untrusted binaries and route their standard streams over a network interface.

## System Architecture

Aether is composed of four primary subsystems, built modularly to interact with specific kernel APIs:

1. **The Supervisor (Process Lifecycle)**
   Manages the birth, execution, and reaping of worker processes using native `fork()`, `execve()`, and `waitpid()` syscalls. It handles signal trapping (e.g., `SIGCHLD`) to prevent zombie processes and ensures clean teardowns.

2. **The IPC Router (Inter-Process Communication)**
   Captures standard output and standard error from the isolated workers. It utilizes unidirectional `pipe()` channels and manipulates the kernel's file descriptor table via `dup2()` to stream data directly into Aether's memory space without hitting the host disk.

3. **The Jailer (Isolation & Security) - *In Development***
   Traps the worker process in a highly restricted sandbox. It utilizes Linux Namespaces (via `clone()` flags like `CLONE_NEWPID`, `CLONE_NEWNS`, `CLONE_NEWNET`) and `pivot_root` to ensure the untrusted code cannot observe or modify the host operating system.

4. **The Governor (Resource Management) - *In Development***
   Enforces strict hardware limits to prevent noisy-neighbor problems or host starvation. It interfaces programmatically with the Linux Virtual Filesystem (VFS) to create and configure `cgroups v2`, setting hard limits on memory (`memory.max`) and CPU time (`cpu.max`).



## Current State

Aether is currently in active development. 
* **Phase 1 (Complete):** Process supervision and basic execution.
* **Phase 2 (In Progress):** IPC implementation for real-time stdout/stderr capture.
* **Phase 3 (Pending):** Cgroups v2 integration for resource limiting.
* **Phase 4 (Pending):** Namespace isolation and chroot/pivot_root.
* **Phase 5 (Pending):** Asynchronous TCP gateway using `epoll`.

## Prerequisites

Because Aether relies heavily on low-level Linux APIs, the goal is to make it portable for all main O.S, but as a V1, it's a Linux project.

* **OS:** Unix based (Kernel 5.x or higher recommended for cgroups v2)
* **Compiler:** `gcc` or `clang`
* **Build System:** `make`

## Build Instructions

The build system enforces strict compilation standards (`-Wall -Wextra -Werror -pedantic`).

```bash
# Compile the daemon
make

# Run the daemon (Requires root privileges for namespaces/cgroups)
sudo make run

# Clean build artifacts
make clean