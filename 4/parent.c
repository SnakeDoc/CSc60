/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 4
 *
 */

// parent.c

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <stdbool.h>
#include <signal.h>
#include <unistd.h>

/* prototypes */
void child_signal_handler();
void exit_handler();

pid_t parent_pid;
pid_t *pids;
int **completed;

int subprocess_num, goal_num = 0;
int count, exit_order = 0;
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

   signal ( SIGCHLD, child_signal_handler );
   atexit ( exit_handler );

   printf ( "\n***Parent (PID %d): forks...\n\n", parent_pid );

   // make room for our children pids
   pids = malloc( sizeof(pid_t) * subprocess_num );

   // make room for first dimension of completed array (ie. completed[])
   completed = malloc( sizeof(int) * subprocess_num );
   // make room for the second dimension of completed array (ie. completed[][])
   int c;
   for ( c = 0; c < subprocess_num; c++ ) {
      completed[c] = malloc( sizeof(int) * 4 );
   }

   for ( count = 0; count < subprocess_num; count++ ) {
      
      pids[count] = fork();
      completed[count][0] = pids[count];
      if ( pids[count] == -1 ) {
         perror ( "fork: " );
         exit(1);
      }
      else if ( pids[count] == 0 ) { // in child process
   
         sprintf ( count_str, "%d", count );
         sprintf ( goal_num_str, "%d", goal_num );

         printf ( "Execl: child #: %s, goal_num: %s\n", count_str, goal_num_str );
         fflush ( stdout );
 
         execl ( "./child", "child", count_str, goal_num_str, 0 );

      }

   }

   sleep(1);
   printf ( "\n***Parent (PID %d): loops for signal for plays...\n\n", parent_pid );

   // round robin loop to control child process sync
   while ( 1 ) {

      for ( count = 0; count < subprocess_num; count++ ) {

         if ( exit_order == subprocess_num ) exit(0);

         if ( completed[count][1] != true ) {

            printf ( "Parent: signal child #%d.\n", count );

            kill ( pids[count], SIGINT );
            sleep(1);

         }

      }

   }

   return 0; // normal return

}

void child_signal_handler() {

   pid_t exit_status, wait_pid;
   int i;

   wait_pid = wait ( &exit_status );

   exit_status <<= 16; exit_status >>= 24;

   for ( i = 0; i < subprocess_num; i++) {

      if ( completed[i][0] == wait_pid ) {

         completed[i][1] = true;
         completed[i][2] = exit_order++;
         completed[i][3] = exit_status;

      }

   }

   printf ( "Parent: child (PID %d) exited, code (sum) %d\n\n", wait_pid, exit_status );

}

void exit_handler() {

   int j, k =0;

   printf ( "\nParent (PID %d): All children completed...\n\n", getpid() );
   printf ( "Completed Order: \n" );

   while ( 1 ) {

      for ( j = 0; j < subprocess_num; j++ ) {

         if ( completed[j][2] == k ) {

            printf ( "\tChild #%d (PID %d), final sum %d\n", j, completed[j][0], completed[j][3] );
            k++;

         }

      }

      if ( k == subprocess_num ) break;

   }

   printf ( "\n" );

}


