#include <stdio.h>

// function declaration
int add(int x, int y);


int main(void)
{
    printf("Hello, World\n");
    
    // single line comment
    
    /*
    block
    comment
    */

    // character type
    char c = '8'; // 1 byte

    // integral type
    short s = 16; // 2 bytes
    int i = 32; // 4 bytes
    long l = 64; // 8 bytes

    // floating point type
    float f = 32; // 4 bytes
    double d = 64; // 8 bytes
    long double ld = 128; // 16 bytes

    //constants
    #define MAX1 10
    const int MAX2 = 10;
    enum {MAX3 = 10};

    // array
    char a[10];
    int pi[5][6];

    // string
    char st[] = "Hello, World\n";

    // conditional statements
    int j = 10; 
    if (j > 5)
    {
        printf("input larger than 5\n");
    }
    else if (j < 0)
    {
        printf("input smaller than 0\n");
    }
    else
    {
        printf("input between 0 and 5\n");
    }
    
    int k = 3;

    switch (k)
    {
    case 1:
        printf("case1\n");
        break;

    case 2:
        printf("case2\n");
        break;    

    default:
        printf("other\n");
        break;
    }

    // loop statements
    for (int i = 0; i < 10; i++)
    {
        printf("for loop number %d\n", i + 1);
    }

    i = 10;
    while (i > 0)
    {
        printf("while loop number %d\n", 11 - i);
        i--;
    }

    // function call
    printf("sum of %d and %d is %d\n", 3, 5, add(3, 5));

}


// function definition
int add(int x, int y)
{
    return x + y;
}


