#include <stdio.h>

int main(void)
{
    // infinite loop
    int i, j;
    i = j = 100;

    for (;;)
    {
        i--;
        if (i = 1)
        {
            printf("loop 100\n");
            break;
        }
    }

    while (1)
    {
        j--;
        if (j = 1)
        {
            printf("loop 100\n");
            break;
        }
    }


    // constants
    enum NUM1{num1, num2, num3}; // 0, 1, 2 
    printf("NUM: %d, %d, %d\n", num1, num2, num3);

    enum NUM2{num4 = 10, num5, num6}; // 10, 11, 12
    printf("NUM: %d, %d, %d\n", num4, num5, num6);

    #define num7 = 1;

    const int num8 = 1;


    
    

}