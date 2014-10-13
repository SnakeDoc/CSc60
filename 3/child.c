/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 3
 *
 */

// child.c

#include <stdio.h>
#include <stdlib.h>

int counter, sum, max = 0;

int main( int argc, char *argv[] )
{

   if ( argc != 3 ) {
      printf ( "Usage: %s <goal>\n", argv[0] );
      exit(1);
   }

   max = atoi ( argv[2] );

   // seed random generator with unique pid
   srand ( getpid() );

   while ( 1 ) {

      sum += rand() % 10;

      printf ( "Child %s here %d, sum is now %d.\n", argv[1], getpid(), sum );
      
      if ( sum >= max ) {
         exit(sum);
      }

      sleep(1);

   }

}


