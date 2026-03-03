#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h> 
#include <sys/types.h>
#include <signal.h>

#include "server.h"
#include "cli.h"
#include "utils.h"

enum AppType {
    CLI,
    SERVER
};

int main(int argc, char *argv[]) 
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

    printf("App type: %u", appType);
    // printf("%s", argv[0]);
    // Fork();
    // Fork();
    // printf("Hello, world from : %d\n", getpid());

    return 0;
}
