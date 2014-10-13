// ptr1.c
// array can be accessed like pointer
//
// x[i] and *(x+i) are the same memory location
// as parameter of subroutine, "int x[]" is the same as "int *x"

#include <stdio.h>
#include <stdlib.h>

#define N 10           // size of array (why fixed?)

void Sub1(int [N]);
void Sub2(int *);

main() {
   int i;
   int x[N] = {40,51,62,73,84,95,106,117,128,139};
   int *p, *q;

   printf("0. %d\n", sizeof(int [0]));
   printf("1. %d\n", sizeof(int [N]));

   p = x;
   q = p;

   printf("2.1. *x is %d, x is %u\n", *x, x);

   printf("2.2. value of q is %u, as addr q points at %d\n", q, *q);

   q = &x[0];
   *q = 10;

   printf("3. value %d  x+1 is %u\n", *(x+1), x+1);
   printf("4. value %d  p+1 is %u\n", *(p+1), p+1);
   printf("5. value %d  q+1 is %u\n", *(q+1), q+1);

   for(i=0; i<N; i++)
      printf("6. main: value %d in addr %u\n", q[i], q+i);

   for(i=0; i<N; i++)
      printf("7. main: value %d in addr %u\n", x[i], x+i);

   Sub1(x);

   for(i=0; i<N; i++)
      printf("9. main: value %d in addr %u\n", *(x+i), x+i);

   Sub2(x);
}

void Sub1(int y[N]) {
   int j;

   for(j=0; j<N; j++)
      printf("8. Sub1: value %d in addr %u\n", *(y+j), y+j);

   y[0] = 999;
}

void Sub2(int *z) {
   int k;

   for(k=0; k<N; k++)
      printf("10. Sub2: value %d in addr %u\n", *(z+k), z+k);

   printf("11. z[3] is %d\n", z[3]);
}

