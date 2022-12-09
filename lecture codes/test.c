#include <stdio.h>

int main(void)
{
    unsigned int x = -4;
    printf("unsigned\n");
    printf("0x%x\n", x);
    printf("0x%x\n", x>>2);
    printf("0x%x\n", x<<1);

    int y = -4;
    printf("signed\n");
    printf("0x%x\n", y);
    printf("0x%x\n", y>>2);
    printf("0x%x\n", y<<1);

    // sign bit expansion occurs if the integer being expanded is signed 
    char a = -1; // a = 1111_2
    unsigned char b = a; // b = 1111_2
    printf("%d\n", (int)a); // sign bit expansion, 1111...11111
    printf("%d\n", (int)b); // no sign bit expansion, 0000...01111
    printf("%u\n", (unsigned int)a); // sign bit expansion, 1111...11111
    printf("%u\n", (unsigned int)b); // no sign bit expansion, 0000...01111

    int c = a; // sign bit expansion
    unsigned int d = a; // sign bit expansion
    printf("%d\n", c); 
    printf("%d\n", d); 
    printf("%u\n", d);

    short ms = -1; // ffffffff
    int mi = -1;
    short s = 1;
    int i = 1;
    unsigned short mus = -1; //ffff
    unsigned int mui = -1;
    unsigned short us = 1;
    unsigned int ui = 1;

    printf("%d\n", ms == mus); // 0
    printf("%d\n", mi == mui); // 1
    printf("%d\n", s == us); // 1
    printf("%d\n", i == ui); // 1

    printf("%d\n", ms == mi); // 1
    printf("%d\n", mus == mui); // 0
    printf("%d\n", s == i); // 1
    printf("%d\n", us == ui); // 1       

    printf("%lx\n", ms); // ffffffff
    printf("%u\n", sizeof(ms)); // 2
    printf("%lx\n", mi);    
    printf("%lx\n", mus);  
    printf("%lx\n", mui);  
}