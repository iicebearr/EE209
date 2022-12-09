#include <stdio.h>

int main(void)
{
    // pointer declaration
    int *k;
    // integer pointer k
    // k is a pointer that points to a integer type variable


    // pointer initialization
    // for a pointer p
    // p: memory address where p points to
    // *p: value of what p points to
    // &p: memory address of p 
    int i = 1;
    int *p = &i;
    printf("value of i: %d\n", *p);

    int j = 10, *q = &j;
    printf("value of j: %d\n", *q);
    *q = 5;
    printf("value of j: %d\n", j);

    int x, *l;
    l = &x;
    scanf("%d", l); // provides scanf the address of variable x
    printf("value of x: %d\n", x);


    // array name can be used as a pointer
    // array name cannot be assigned a new value
    int b[10];
    *b = 10;
    *(b + 1) = 20;
    printf("b[0]=%d\n", b[0]);
    printf("b[1]=%d\n", b[1]);    


    // adding an integer to a pointer
    int a[5]={0, 1, 2, 3, 4}, *t, *u;
    t = a; // address of a[0]
    printf("a[0]=%d\n", *t);
    printf("a[1]=%d\n", *(t + 1));
    u = t + 2;
    printf("a[2]=%d\n", *u);

    // complex pointer declaraion
    int A[3] = {1, 2, 3}, B[3] = {4, 5, 6};
    int (*A3)[3], (*B3)[3], *pA; // pointer that points to an array of 3 integers
    A3 = &A;
    B3 = &B;

    // *A3 = *B3; // compile error as it’s the same as A = B
    **A3 = **B3; // fine, same as A[0] = B[0]
    pA = *A3; // pA = A
    printf("A[0]: %d\n", pA[0]);
    printf("A[1]: %d\n", pA[1]);
    printf("A[2]: %d\n", pA[2]);

    **A3 = 10; // fine, same as A[0] = B[0]
    printf("A[0]: %d\n", pA[0]);
    printf("B[0]: %d\n", **B3);
}