#define _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>
#include <fcntl.h>


int main(void)
{
    /* creating a new process */ 
    // fork() is called once
    // but returns twice, once in each process
    // returns 0 for child, and returns child's process ID for parent

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

    pid_t pid = fork();
    if (pid != 0) {
    /* in parent */
    wait(NULL); // Waiting for a child to terminate
    waitpid(pid, NULL, 0); // Waiting for specific child to terminate
    } else {
    /* in child */
    char *arguments[2];
    arguments[0] = "ls";
    arguments[1] = NULL;
    execvp("ls", arguments); // invoke new process
    fprintf(stderr, "exec failed\n");
    exit(EXIT_FAILURE);
    }

    // Single call that combines all three fork(), exec(), wait()
    system("echo Hello world");


    /* redirection */
    // Use open(), creat(), and close() system calls
    // and dup(), dup2() system calls to manage file descripter table
    
    // int dup(int oldfd)
    // Create a copy of the file descriptor oldfd and
    // uses the lowest-numbered unused descriptor for the new descriptor
    // Return the new descriptor
    // They refer to the same open file description and
    // thus share "file offset" and "file status flags"

    // int dup2(int oldfd, int fd)
    // can designate which fd to replace

    pid = fork();
    if (pid == 0) {
    /* in child */
    int fd = creat("output.txt", 0640);
    close(1);  // close stdout
    dup(fd);   // duplicate fd to 1 
    close(fd); // close fd
    char *arguments[2];
    arguments[0] = "ls";
    arguments[1] = NULL;
    execvp("ls", arguments); // invoke new process
    fprintf(stderr, "exec failed\n");
    exit(EXIT_FAILURE);
    }
    else {
    /* in parent */
    pid = wait(NULL);
    }
}