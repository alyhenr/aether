# 📜 Product Requirements Document: Project Aether

**Project:** Aether Runtime Engine
**Status:** In Development
**Language:** C (C99/C11)
**Target OS:** Linux (Kernel 5.x+)

## Part 1: High-Level PRD (User Perspective)

### 1. Product Vision
Aether is a high-performance, bare-metal Function-as-a-Service (FaaS) engine. It allows users to submit untrusted executable code over a network and guarantees that the code will run in a strictly isolated, resource-constrained environment. 

It does not rely on heavy abstractions like Docker or Kubernetes. It is a single, lightweight C daemon that interacts directly with the Linux kernel to create ephemeral micro-worlds for every execution.

### 2. Target Use Cases
* **Coding Interview Platforms:** Executing user-submitted algorithmic solutions safely.
* **CI/CD Runners:** Running build scripts without risking the host machine.
* **Agentic AI Backends:** Providing a secure "sandbox" for AI agents to run generated code and return the output.

### 3. User Experience (The Execution Flow)
From the perspective of an external developer interacting with Aether:

1.  **Submission:** The user sends a TCP payload containing a binary or script to Aether's listening port.
2.  **Provisioning (Invisible to User):** Within milliseconds, Aether creates a pristine, jailed environment.
3.  **Execution:** The user's code runs. It cannot access the internet, it cannot see the host filesystem, and it cannot consume more than its allotted RAM/CPU.
4.  **Termination:** The code finishes (or is forcefully killed if it misbehaves).
5.  **Response:** The user receives a JSON/text response containing:
    * The `stdout` and `stderr` of their program.
    * The exit code (e.g., `0` for success, `137` for OOM kill).
    * Telemetry (execution time in ms, peak memory usage in MB).

---

## Part 2: Low-Level Technical PRD (Engineering Guide)

### 1. System Architecture & Subsystems

#### A. The Gateway (Network & Concurrency)
* **Role:** Handle incoming network connections without blocking the main thread.
* **Mechanism:** An Event Loop using `epoll`.
* **Requirements:**
    * Accept incoming TCP connections.
    * Read payloads into memory safely (prevent buffer overflows).
    * Use non-blocking sockets (`O_NONBLOCK`).

#### B. The Supervisor (Process Lifecycle)
* **Role:** Spawn the worker process, wait for it to finish, and capture its output.
* **Mechanism:** `fork()`, `execve()`, `waitpid()`, and `pipe()`.
* **Requirements:**
    * Create anonymous pipes before forking to redirect the child's `stdout`/`stderr`.
    * Handle `SIGCHLD` to reap finished processes and prevent zombies.
    * Implement a strict timeout mechanism.

#### C. The Jailer (Security & Isolation)
* **Role:** Ensure the child process cannot harm or observe the host.
* **Mechanism:** Linux Namespaces and `pivot_root`.
* **Requirements:**
    * Use `clone()` with flags: `CLONE_NEWPID`, `CLONE_NEWNS`, `CLONE_NEWNET`.
    * Prepare a minimal root filesystem (rootfs).
    * Use `pivot_root` to trap the process.
    * Drop root privileges inside the jail.

#### D. The Governor (Resource Management)
* **Role:** Prevent the child process from starving the host of CPU or RAM.
* **Mechanism:** Cgroups v2.
* **Requirements:**
    * Programmatically create a new directory in `/sys/fs/cgroup/aether_workers/`.
    * Write memory/CPU limits to `memory.max` and `cpu.max`.
    * Read execution metrics after the process dies.

### 2. Technical Specifications

| Component | Specification |
| :--- | :--- |
| **Language** | C (compiled with `gcc` or `clang`, `-Wall -Wextra -Werror`) |
| **Concurrency** | Single-threaded Event Loop (`epoll`) + Multi-process Workers |
| **IPC** | Anonymous Pipes (`pipe()`) |
| **Sandbox** | Linux Namespaces, Cgroups v2, chroot/pivot_root |

### 3. Implementation Phases (The Roadmap)

1.  **Phase 0: Project Scaffolding.** Setup `Makefile`, basic TCP server, and logging.
2.  **Phase 1: The Raw Spawner.** The server receives a command, forks, execs, and captures stdout via pipes.
3.  **Phase 2: The Cgroup Governor.** The server enforces memory/CPU limits on the raw spawned process.
4.  **Phase 3: The Namespace Jail.** The server swaps `fork` for `clone` and traps the process.
5.  **Phase 4: Polish.** Add telemetry reporting and rigorous error handling.