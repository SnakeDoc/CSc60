/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 3
 *
 */

// parent.c

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <signal.h>
#include <unistd.h>

pid_t parent_pid, exit_status, wait_pid;
pid_t *pids;

int subprocess_num, goal_num = 0;
int count = 0;
char count_str[3];
char goal_num_str[3];

int main( int argc, char *argv[] )
{

   if ( argc != 3 ) {
      printf ( "Usage: %s <number of processes> <goal>\n", argv[0] );
      exit(1);
   }

   subprocess_num = atoi(argv[1]);
   goal_num = atoi(argv[2]);

   parent_pid = getpid();
   printf ( "\nParent here %d, about to fork...\n\n", parent_pid );

   pids = malloc( sizeof(pid_t) * subprocess_num ); // make room for our children pids

   for ( count = 0; count < subprocess_num; count++ ) {
      
      pids[count] = fork();
      if ( pids[count] == -1 ) {
         perror ( "fork: " );
         exit(1);
      }
      else if ( pids[count] == 0 ) { // in child process
   
         sprintf ( count_str, "%d", count );
         sprintf ( goal_num_str, "%d", goal_num );

         execl ( "./child", "child", count_str, goal_num_str, 0 );

      }

   }

   // wait for winner, the kill children processes
   wait_pid = wait ( &exit_status );
   for ( count = 0; count < subprocess_num; count++ ) {

      if ( pids[count] != wait_pid ) {
         kill ( pids[count], SIGKILL );
      }

   }

   // get exit code
   exit_status <<= 16; exit_status >>= 24;
   printf ( "\nParent here %d: Child %d was the winner with sum: %d\n\n", parent_pid, wait_pid, exit_status );

   return 0; // normal return

}




