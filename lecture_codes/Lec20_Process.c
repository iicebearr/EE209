#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>


int main{
    /* creating a new process */ 
    // fork() is called once
    // but returns twice, once in each process
    // returns 0 for child, and returns child's process ID for parent
    pid_t pid = fork();
    if (pid != 0) {
    /* in parent */
    wait(NULL);
    waitpid(pid);
    } else {
    /* in child */
    }

    print(asd)


    /* Inherited from parent to child*/ 
    // User and group IDs
    // Signal handling settings
    // stdio
    // File pointers!!!!!!!!!!!!!!
    // Root directory
    // File mode creation mask
    // Resource limits
    // Controlling terminal
    // All machine register states
    // Control register(s)

    /* Separate in child */ 
    // Process ID
    // Address space (memory)!!!!!!!!!!!
    // File descriptors!!!!!!!!!!!!!
    // Parent process ID
    // Pending signals
    // Time signal reset times
    dsad
    dad
    a
    d
}