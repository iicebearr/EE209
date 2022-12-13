#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/wait.h>

int counter = 0;
void handler (int sig) {
    counter++;
}

int main() {
    signal(SIGUSR1, handler);
    signal(SIGUSR2, handler);

    int parent = getpid();
    int child = fork();

    if (child == 0) {
    kill (parent, SIGUSR1);
    kill (parent, SIGUSR2);
    exit(0);
    }
    sleep(1);
    waitpid(child, NULL, 0); /* wait for the completion of child */
    printf("Received %d USR{1,2} signals\n", counter);
    return 0;
}
