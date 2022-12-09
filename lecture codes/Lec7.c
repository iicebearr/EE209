#include <stdio.h>
#include <stdlib.h>

int main(void)
{
    /* STRUCTURES */
    // structure declaration, initialization
    struct student {
        int number;
        char *name;
    } part1, //structure variables
      part2 = {2, "EJ"},
      *part_ptr;

    // specify the name of the memeber to select it
    part1.number = 1;
    printf("part1.number = %d\n", part1.number);
    printf("part2.number = %d\n", part2.number);

    // 2 ways for pointer
    part_ptr = &part1;
    (*part_ptr).number = 10;
    part_ptr->name = "PTR"; // member 'name' of what's pointed to 'part_ptr' 
    printf("(*part_ptr).number = %d\n", (*part_ptr).number);
    printf("part_ptr->number = %d\n", part_ptr->number);

    // copy all members by assignment
    // except assignment, no other operations on entire structures are allowed
    part1 = part2;
    printf("part1.number = %d\n", part1.number);
    printf("part1.name = %s\n", part1.name);

    // using structure to copy array contents
    struct { char a[10]; } a1, a2 = {"123"};
    a1 = a2;
    printf("a1.a = %s\n", a1.a);

    // using structure tag to declare
    struct student part3 = {3, "EJK"};

    // using typedef to define a structure type
    typedef struct {
        int price;
        char name[10];
    } product;

    typedef struct {
        struct student person;
        product item;
    } customer;
    
    product prod1 = {1000, "Gum"};
    customer cus1;
    cus1.person.number = 3;
    cus1.person.name = "HW";


    /* UNIONS */
    union number
    {
        int i;
        double d;
    } u1, u2;

    u1.i = 5;
    u2.d = 1.5;


    /* DYNAMIC STORAGE ALLOCATION */
    // malloc: allocates a block of memory but doesn’t initialize it
    // calloc: allocates a block of memory and clears it
    // realloc: resizes a previously allocated block of memory

    // If a memory allocation function can’t locate
    // a memory block of the requested size, it returns a null pointer

    // allcocate a string of 100 characters
    char *p;
    if ((p = (char *)malloc(100 + 1)) == NULL) // 100 Bytes of memory
    {
    /* allocation failed; take appropriate action */
    } 

    // p is pointer
    char s[4] = "123";
    p = s;
    printf("p = %s\n", p);

    // calloc has two parameters: 
    // 1. number of elements, 2. size of each element
    // calloc initializes each block to zero
    int n = 100;
    int *a = (int *)calloc(n, sizeof(int)); // n * sizeof(int) Bytes of memory
    printf("a[10] = %d\n", a[10]);

    // realloc
    // used after malloc or calloc
    void *q = malloc(100);
    q = realloc(q, 50); // in case realloc changes memory address

    // free: deallocate storage
    void *p1 = malloc(10);
    void *q2 = malloc(10);
    free(p1);
    p = q;



    

}
