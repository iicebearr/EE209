#include <stdio.h>
#define NELEMS(x) (sizeof(x)/sizeof(x[0]))

int main( ) {
    int i;
    int y[10];
    printf(NELEMS(y));
    //for (i = 10; i - NELEMS(y) >= 0; i-= 10)
    //    printf("current i is %d\n", i);
    return 0;
}