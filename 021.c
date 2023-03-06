// The purpose of this test is to check calls for again() from main.
// This test also checks if you have the proper exit code even if
// again() is called with main

#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <sys/wait.h>
#include <sys/types.h>
#include "src/go.h"

#define EXIT_CODE 237

int x = 10;
bool forked = false;
Channel* ch;

Value test() {
    receive(ch);
    again();
    return asInt(0);
}

int main() {
    pid_t pid = 0;
    if (!forked) {
        forked = true;
        pid = fork(); // create only one child process
    }
    if (pid == 0) {
        // in the child process do some again() calls with main
        if (x == 10)
            ch = go(test);
        send(ch, asInt(x));
        printf("%d\n", x--);
        if (x >= 0)
            again();
        return EXIT_CODE;
    }
    // in the parent process check if the child process gave the
    // correct exit code even with again() calls in main
    int wstatus;
    waitpid(pid, &wstatus, 0);
    if (WEXITSTATUS(wstatus) != EXIT_CODE)
        printf("Did not preserve exit code in main\n");
    else
        printf("Success!\n");
}
