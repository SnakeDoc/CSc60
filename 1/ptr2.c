// ptr2.c

#include <stdio.h>

Change0(int *p) { *p = 2; } // can change the value in the addr of what p has

Change1(int *p) { int i = 10; p = &i; } // can't change the addr p points to to i's

Change2(int **dog) { int i = 20; *dog = &i; } // unless p's addr is passed over

Change3(int *p) { *p = 30; } // can change value in the addr of what p has

Change4(int *p) { int i = 40; p = &i; } // can't change addr p points to to i's

Change5(int **p) { int i = 50; *p = &i; } // unless p's addr is passed over

ChangeX(int a) { a = 500; }

main() {
   int i, j, *p;

   i = 1; p = &i;

   printf("1. Before: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);
   ChangeX( i );

   Change0( p );
   printf("2. Change0: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);

   Change1( p );
   printf("3. Change1: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);

   Change2( &p );
   printf("4. Change2: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);

   j=3; p=&j;

   printf("5. Before: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);
   Change3( p );
   printf("6. Change3: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);

   Change4( p );
   printf("7. Change4: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);

   Change5( &p );
   printf("8. Change5: *p=%u  p=%u  &p=%u\n\n", *p, p, &p);
}

