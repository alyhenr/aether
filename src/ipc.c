#include <stdio.h>
#include <unistd.h>
#include <utils.h>

#define BUF_SIZE 10
/*
 * Creates a unidirectional data channel.
 * Populates pipefd[0] for reading and pipefd[1] for writing.
 * Returns 0 on success, -1 on failure.
 */
int ipc_init_pipes(int pipefd[2])
{
    if (pipe(pipefd) == -1) {
        unix_err("pipe");
    }

    return 0;
}

/*
 * STRICTLY FOR THE CHILD PROCESS.
 * Replaces the child's standard output (FD 1) with the pipe's write-end.
 * Closes the unused read-end to prevent file descriptor leaks.
 */
void ipc_child_wire_stdout(int pipefd[2])
{
    if (close(pipefd[0]) == -1) {
        unix_err("[WORKER] Close read end of pipe");
    }
    
    if (dup2(pipefd[1], STDOUT_FILENO)  == -1) { //duplicate write end of the pipe onto the standard output file descriptor
        unix_err("[WORKER] Duplicate write enf of pipe");
    }


    printf("Hello World from the CHILD VIA IPC!\n");
    fflush(stdout);

    if (close(pipefd[1]) == -1) {
        unix_err("[WORKER] Close write end of pipe");
    }
}

/*
 * STRICTLY FOR THE PARENT PROCESS.
 * Closes the parent's unused write-end, then blocks in a while-loop,
 * reading bytes from the pipe and printing them until the child closes it.
 */
void ipc_parent_capture_output(int pipefd[2])
{
    if (close(pipefd[1]) == -1) {
        unix_err("[SUPERVISOR] Close write end of pipe");
    }

    int num_read;
    char buf[BUF_SIZE];
    for(;;) {
        num_read = read(pipefd[0], buf , BUF_SIZE);

        if (num_read == -1) {
            unix_err("read");
        }
        if (num_read == 0)  
            break; 
        if (write(STDOUT_FILENO, buf, num_read) != num_read) {
            unix_err("[SUPERVISOR] Partial write");
        }
        
    }


    if (close(pipefd[0]) == -1) {
        unix_err("[SUPERVISOR] Close read end of pipe");
    }
}
