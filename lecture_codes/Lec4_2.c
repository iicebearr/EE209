#include <stdio.h>

void inc() 
{
    // "static local variable"
    // lexical scope: local variable
    // temporal scope: global variable
    static int sum = 0;
    sum++;
}

// "global variable"
int GobalVar = 1;

// "static gobal variable"
// lexical scope: local to a file where it is defined
// temporal scope: global variable
static int StaticGobalVar = 1;

int i = 1; /* Declaration 1 */

void f(int i) /* Declaration 2 */
{
    printf("f: %d\n", i);
}

void g(void)
{
    int i = 100; /* Declaration 3 */

    if (i > 99) {
    int i = 1000; /* Declaration 4 */
    printf("g1: %d\n", i);
    }

    printf("g2: %d\n", i);
}

void h(void)
{
    printf("h: %d\n", i);
}



int main(void)
{
    // "local variable"
    int LocalVar = 1;

    // "static local variable"
    static int StaticLocalVar = 1;

    inc(); // sum = 1
    inc(); // sum = 2
    //sum++; <- error, lexical scope: local variable

    f(10);
    g();
    h();
}

