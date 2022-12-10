#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

// stream: any source of input or destination for output
// accessed in c programs through file pointers

// three streams are provided by stdio.h
// stdin, stdout, stderr

// standard I/O functions wrap system calls
// standard I/O functions provide additional functionalities such as buffered I/O

// file descriptor: an integer that uniquely identifies an open file

// standard I/O functions use file pointers that points to FILE structure
// system calls use file descripters
// file descripters are within the FILE structure

int main(void)
{
    /* standard I/O functions */
    // fopen(), fclose(), fflush(),
    // fprintf(). fscanf(),
    // getchar(), fgets(), etc.

    int i = 1;
    char text[100];

    FILE *fp1 = fopen("mytext.txt", "w");
    fprintf(fp1, "Number:%d\n", i);
    fflush(fp1);

    FILE *fp2 = fopen("mytext.txt", "r");
    fscanf(fp2, "%s", text);
    fprintf(stdout, "mytext.txt: %s\n", text);

    fclose(fp1);
    fclose(fp2);


    /* "system calls" for I/O */
    // int creat(char *pathname, mode_t mode);
    //      Creates a new file named pathname, and returns a file descriptor
    //      mode: permissions to set

    // int open(char *pathname, int flags, mode_t mode);
    //      Opens the file pathname and returns a file descriptor
    //      flags: O_RDONLY, O_WRONLY, )_RDWR
    //      mode: permissions to set if file is created
    
    // int close(int fd);
    //      Closes fd
    
    // int read(int fd, void *buf, int count);
    //      Reads up to count bytes from fd into the buffer at buf
    //      Returns the number of bytes read
    //      file offset is incremented by the number of bytes read
    
    // int write(int fd, void *buf, int count);
    //      Writes up to count bytes into fd from the buffer at buf
    
    // int lseek(int fd, int offset, int whence);
    //      Assigns the file pointer of fd to a new value by applying an offset


    /* bufferd I/O */
    printf("Step 1\n");
    fflush(stdout); // flush the output buffer
    sleep(3);
    printf("Step 2\n");
}