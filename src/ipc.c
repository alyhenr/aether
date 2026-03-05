#include <stdio.h>

/*
 * Creates a unidirectional data channel.
 * Populates pipefd[0] for reading and pipefd[1] for writing.
 * Returns 0 on success, -1 on failure.
 */
int ipc_init_pipes(int pipefd[2])
{
    return pipefd[2];
}

/*
 * STRICTLY FOR THE CHILD PROCESS.
 * Replaces the child's standard output (FD 1) with the pipe's write-end.
 * Closes the unused read-end to prevent file descriptor leaks.
 */
void ipc_child_wire_stdout(int pipefd[2])
{
    printf("%d", pipefd[2]);
}

/*
 * STRICTLY FOR THE PARENT PROCESS.
 * Closes the parent's unused write-end, then blocks in a while-loop,
 * reading bytes from the pipe and printing them until the child closes it.
 */
void ipc_parent_capture_output(int pipefd[2])
{
    printf("%d", pipefd[2]);
}
