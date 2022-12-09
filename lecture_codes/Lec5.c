#include <stdio.h>

void print(int a[]){
    for (int i = 0; i < 5; i++)
        printf("[%d]=%d\n", i, a[i]);
    printf("\n");
}


int main(void)
{
    // array declaration
    int a[10];
    char c[10];
    float f[10];

    // array initialization
    int b[5] = {1, 2, 3, 4, 5};
    print(b);

    int d[5] = {1, 2, 3};
    print(d);

    int e[] = {1, 2, 3, 4, 5};
    print(e); 

    printf("%d\n", sizeof(e));
    printf("%d\n", sizeof(e[1]));
    printf("\n");

    // multidimensional array
    int g[2][2]={{1,2}, {3}};
    printf("%d\n", g[0][0]);
    printf("%d\n", g[0][1]);
    printf("%d\n", g[1][0]);
    printf("%d\n", g[1][1]);
    printf("\n");

    // string
    char x[] = "abc";
    printf("%s\n", x);
    printf("%c\n", x[3]); // null cahracter '\0'
    printf("\n");

    // string literal
    printf("Hello\n"); // "Hello\n" is a string literal(string constant)
    printf("%c\n", "Hello"[1]); // e
    printf("\n");

    //char pointer
    char *p = "Hello";
    printf("%o\n", p); // address of 'H' of "Hello", 8 bytes
    printf("%c\n", p[1]); // 'e'
    printf("\n");

    // char array vs. char pointer
    char date1[8] = "June 14"; // char array
    char *date2 = "June 14"; // char pointer that points to a string constant

    date1[3] = 'K';
    printf("%s\n", date1);
    // date2[3] = 'K';

    // date1++;
    // date1 += 2;
    date2++; // date2 = date2 + 1, date2 now stores address of 'u'
    printf("%s\n", date2);
    date2 += 2; // date2 = date2 + 2, date2 now stores address of 'e'
    printf("%s\n", date2);

    date2 = date1;
    date2[2] = 'J' ;
    printf("%s\n", date1);
}