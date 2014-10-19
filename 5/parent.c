/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 5
 *
 */

// parent.c

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>

/* prototypes */
void exit_handler();

pid_t parent_pid;
pid_t **pids;

int **num_pipe;
int **sum_pipe;

int subprocess_num, goal_num = 0;
int count, exit_order = 0;
char count_str[3];
char goal_num_str[3];

int main( int argc, char *argv[] )
{

   char read_pipe_str[8];
   char write_pipe_str[8];
   int random_num;
   int temp_sum;

   if ( argc != 3 ) {
      printf ( "Usage: %s <number of processes> <goal>\n", argv[0] );
      exit(1);
   }

   subprocess_num = atoi(argv[1]);
   goal_num = atoi(argv[2]);

   parent_pid = getpid();

   atexit ( exit_handler );

   printf ( "\nParent: my pid %d, about to fork...\n\n", parent_pid );

   // seed random number generator with unique pid
   srand ( parent_pid );

   // make room for our children pids
   //
   // pids[] = Child Number
   //
   // pids[count][0] = pid
   // pids[count][1] = is completed (true/false)
   // pids[count][2] = exited order
   // pids[count][3] = sum
   //
   // make room for first dimension of pids array (ie. pids[])
   pids = malloc( sizeof(pid_t) * subprocess_num );
   // make room for the second dimension of pids array (ie. pids[][])
   int c;
   for ( c = 0; c < subprocess_num; c++ ) {
      pids[c] = malloc( sizeof(pid_t) * 4 );
   }

   // create pipes 2 dimensional array
   //
   // pipes[] = Child Number
   //
   // pipes[count][0] = Read end of pipe
   // pipes[count][1] = Write end of pipe
   //
   // first dimension
   num_pipe = malloc( sizeof(int) * subprocess_num );
   sum_pipe = malloc( sizeof(int) * subprocess_num );
   // second dimension
   for ( c = 0; c < subprocess_num; c++ ) {
      num_pipe[c] = malloc( sizeof(int) * 2 );
   }
   for ( c = 0; c < subprocess_num; c++ ) {
      sum_pipe[c] = malloc( sizeof(int) * 2 );
   }

   // create pipes
   for ( c = 0; c < subprocess_num; c++ ) {

      // num pipe
      if ( -1 == pipe ( &num_pipe[c][0] ) ) {
         perror ( "pipe: ");
         exit(1);
      }

      // sum pipe
      if ( -1 == pipe ( &sum_pipe[c][0] ) ) {
         perror ( "pipe: " );
         exit(1);
      }

      printf ( "Parent: num_pipe[%d][0] is %d...\n", c, num_pipe[c][0] );
      printf ( "Parent: sum_pipe[%d][1] is %d...\n", c, sum_pipe[c][1] );

   }

   // fork and execl children
   for ( count = 0; count < subprocess_num; count++ ) {
      
      pids[count][0] = fork();
      if ( pids[count][0] == -1 ) {
         perror ( "fork: " );
         exit(1);
      }
      else if ( pids[count][0] == 0 ) { // in child process
   
         sprintf ( count_str, "%d", count );
         sprintf ( goal_num_str, "%d", goal_num );
         sprintf ( read_pipe_str, "%d", num_pipe[count][0] );
         sprintf ( write_pipe_str, "%d", sum_pipe[count][1] );

         execl ( "./child", "child", count_str, goal_num_str, read_pipe_str, write_pipe_str, NULL );

      }

   }

   sleep(1);

   // round robin loop to control child process sync
   while ( 1 ) {

      for ( count = 0; count < subprocess_num; count++ ) {

         if ( exit_order == subprocess_num ) exit(0);

         if ( pids[count][1] != true ) {

            random_num = rand() % 10;

            printf ( ">>> Parent: sending child #%d num %d.\n", count, random_num );

            // send number to child process
            write ( num_pipe[count][1], &random_num, sizeof(int) );

            // block until child has returned a new sum
            read ( sum_pipe[count][0], &temp_sum, sizeof(int) );

            printf ( "<<< Parent: reading child #%d returning sum %d.\n", count, temp_sum );

            pids[count][3] = temp_sum;

            if ( pids[count][3] >= goal_num ) {
               pids[count][1] = true;
               pids[count][2] = exit_order++;
            }

         }

      }

   }

   return 0; // normal return

}

void exit_handler() {

   int j, k =0;

   printf ( "*** THE END ***\n");
   printf ( "Parent proceeds to clean up deaths and close pipes...\n");

   for ( j = 0; j < subprocess_num; j++ ) {

      close ( num_pipe[j][0] );
      close ( num_pipe[j][1] );
      close ( sum_pipe[j][0] );
      close ( sum_pipe[j][1] );

   }

   while ( 1 ) {

      for ( j = 0; j < subprocess_num; j++ ) {

         if ( pids[j][2] == k ) {

            printf ( "Child #%d (PID %d), finished at sum %d.\n", j, pids[j][0], pids[j][3] );
            k++;

         }

      }

      if ( k == subprocess_num ) break;

   }

   printf ( "\n" );

}


