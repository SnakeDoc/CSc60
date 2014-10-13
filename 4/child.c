/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 4
 *
 */

// child.c

#include <stdio.h>
#include <stdlib.h>
#include <signal.h>

/* prototypes */
void signal_handler();

int counter, sum, max = 0;
char *childnum_str;

int main( int argc, char *argv[] )
{

   if ( argc != 3 ) {
      printf ( "Usage: %s <goal>\n", argv[0] );
      exit(1);
   }

   childnum_str = argv[1];
   max = atoi ( argv[2] );

   // seed random generator with unique pid
   srand ( getpid() );

   // register our signal handler
   signal ( SIGINT, signal_handler );

   while ( 1 ) {

      sleep(1); // wait for parent signal (SIGINT)

   }

   return 0;

}

void signal_handler()
{
   int num = rand() % 10;
   sum += num;

   printf ( "\tChild #%s (PID %d), got num %d, sum now %d\n\n", childnum_str, getpid(), num, sum );
   fflush(stdout);
 
   if ( sum >= max ) {

      exit(sum);

   }

}

