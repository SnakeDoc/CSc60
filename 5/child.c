/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 5
 *
 */

// child.c

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

int main( int argc, char *argv[] )
{

   int sum = 0;
   int num, max, read_pipe, write_pipe;
   char *childnum_str;

   pid_t childpid = getpid();

   if ( argc != 5 ) {
      printf ( "Usage: %s <goal> <read_pipe> <write_pipe>\n", argv[0] );
      exit(1);
   }

   childnum_str = argv[1];
   max = atoi ( argv[2] );
   read_pipe = atoi ( argv[3] );
   write_pipe = atoi ( argv[4] ); 

   printf ( "\tChild %d starts: child num %s, goal num %d, read pipe %d, write pipe %d.\n",
      childpid, childnum_str, max, read_pipe, write_pipe );

   while ( 1 ) {

      // read from parent random number
      read ( read_pipe, &num, sizeof(int) ); // block until read available

      sum += num;

      printf ( "\tChild: I'm child #%s (pid %d), got num %d, sum becomes %d.\n", 
         childnum_str, childpid, num, sum );

      // send parent the new sum
      write ( write_pipe, &sum, sizeof(int) );

      if ( sum >= max ) exit(sum);

   }

   return 0;

}


