#include <stdio.h>


void startCLI() {
    printf("--- Aether is running ---\n");
    printf("You can start running your binaries in a safe environment.\nWaiting command...\n>");

    int test;
    while (1) {
        // scanf returns the number of items successfully read
        if (scanf("%d", &test) != 1) {
            printf("Invalid input! Please enter a number.\n>");
            
            // Clear the buffer: read characters until we hit a newline
            while (getchar() != '\n');
            
            continue; // Try again
        }
        
        printf("Test variable: %d\n>", test);
    }
}