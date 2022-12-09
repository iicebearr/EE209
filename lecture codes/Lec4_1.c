#include <stdio.h>

// function declaration
// function declaration is assumed as extern by default
void dummy1(int x[]);
void dummy2(int y);
float calculateSum(float num[]);

// #define, #undef
#define DEBUG 0
// #define DEBUG 1
#define TEST


int main(void)
{
    // #if, #elif, #else, #endif
    #if DEBUG == 0
    printf ("DEBUG is on\n") ;
    #else
    printf ("DEBUG is off\n");
    #endif

    // #ifndef, #ifdef, #undef, #endif
    #ifdef TEST
    printf ("TEST is on\n") ;
    #else
    printf ("TEST is off\n") ;
    #endif

    #undef TEST // delete Macro
    #ifndef TEST
    printf ("TEST is off\n") ;
    #else
    printf ("TEST is on\n") ;
    #endif



    int a[] = {100, 200};
    dummy1(a); // pass an array type(actually pointer), array type cannot be passed by value
    printf("a[0]=%d, a[1]=%d\n", a[0], a[1]); // changes a[]
    dummy2(a[1]); // pass an integer type
    printf("a[0]=%d, a[1]=%d\n", a[0], a[1]); // does not change a[]

    int b = 100;
    dummy2(b); // pass an integer type
    printf("b=%d\n", b); //does not change b

    float result, num[] = {23.4, 55, 22.6, 3, 40.5, 18};
    result = calculateSum(num); 
    printf("Result = %.2f", result);
}


// function definition
void dummy1(int x[]){
x[0] = 0;
x[1] = 1;
}

void dummy2(int y) {
y = 2;
}

float calculateSum(float num[]) {
  float sum = 0.0;

  for (int i = 0; i < 6; ++i) {
    sum += num[i];
  }

  return sum;
}
