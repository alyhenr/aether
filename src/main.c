#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <signal.h>

#include "./include/server.h"
#include "./include/cli.h"
#include "./include/utils.h"

enum AppType {
    CLI,
    SERVER
};

int main(int argc, char *argv[], char* envp[]) 
{   
    enum AppType appType;
    if (argc == 1) {
        appType = CLI;
        startCLI();
    } else {
        if (strcmp(argv[1], "SERVER")) {
            appType = SERVER;
            startServer();
        } else {
            appType = CLI;
            startCLI();
        }
    }

    // printf("%s", argv[0]);
    // Fork();
    // Fork();
    // printf("Hello, world from : %d\n", getpid());

    return 0;
}
