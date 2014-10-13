// ptr0.c
//
// compile it in 32-bit mode: gcc -m32 ptr0.c

#include <stdio.h> 

main() {
   int i, j, *p, *q, **r;

   printf("Make sure it's 4 (bytes): size of int * is %i\n",
      sizeof(int *));

// give p an address (p then points there),
// *p = 33 or *p = i are illegal
// p = 0; // 0 is the only value that a ptr can be given directly
   i = 33;
   p = &i;

   printf("1. *p is %u\n", *p); // *p is 33 (%u, unsigned int format)

   printf("2. 2**p is %u\n", 2**p); // means: 2* *p is 66

   printf("3. p has %u, &i is %u\n", p, &i); // p (addr of i), addr of i

   printf("4. p has %u at addr %u\n", p, &p); // addr of i, addr of p

   j = (int)p; // typecast to assign j, not TOWA: (int *) j = p;

   printf("5. *(int *)j is %u, j has %u\n", *(int *)j, j); // 33, addr of i

   printf("6. (int *)j is %u, j has %d\n", (int *)j, j); // both addr of i

//**************************************************************************
   *p = 44;

   printf("7. *p is %u\n", *p);
   printf("8. p has %u at its own addr %u\n", p, &p);

//**************************************************************************
   i = 5;
   p = &i;
   q = p;
   r = &q;

   printf("9. i has %u at addr %u\n",i, &i);

   printf("10. p has %u at addr %u\n", p, &p);
   printf("11. *p is %u\n", *p);

   printf("12. q has %u at addr %u\n", q, &q);
   printf("13. *q is %u\n", *q);

   printf("14. r has %u at addr %u\n", r, &r);
   printf("15. *r has %u\n", *r);
   printf("16. **r has %u\n", **r);
}

