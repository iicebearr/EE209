#include <stdio.h>

int main(void)
{
    unsigned char a = 255;
    unsigned char b = 1;
    unsigned char c = a + b; // c = 0 (256 -> 0) due to overflow
    unsigned char d = b - a; // d = 2 (-254 -> 2) due to overflow


    // type casting
    int sum = 19, count = 5;
    double avg1, avg2;
    avg1 =  sum/count;
    avg2 = (double) sum/count;
    printf("%f, %f\n", avg1, avg2);

    short sx = -1; // signed short, bit pattern: 0xFFFF
    unsigned short ux = sx; // ux = 2^16-1 = 65535
    printf("signed: %hd, unsigned: %hu\n", sx, ux);

    // type casting between signed and insigned constants
    if (-1 < 0)
        printf("1 < 0\n");
    
    if (-1 > 0U) // -1 is type casted to unsigned due to 0U
        printf("-1 > 0U\n");

    // constants
    enum {num1 = 5}; // int by default
    enum {num2 = 5L}; // suffix L to make long
    enum {num3 = (short)5}; // type casting to make short
    enum {num4 = 5U}; // suffix U to make unsigned

    #define num5 = 1.5 // double by default
    #define num7 = 1.5L // suffix L to make long double
    #define num6 = 1.5F // suffix F to make float

    printf("size of int: %lu\n", sizeof(5));
    printf("size of long: %lu\n", sizeof(5L));
    printf("size of short: %lu\n", sizeof((short)5));

    printf("size of double: %lu\n", sizeof(1.5));
    printf("size of long double: %lu\n", sizeof(1.5L));
    printf("size of float: %lu\n", sizeof(1.5F));
    


}