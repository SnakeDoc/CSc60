/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 6
 *
 */

// CR.c

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>
#include <signal.h>

#define _REENTRANT
#define IN fileno(stdin)
#define OUT fileno(stdout)
#define MAX_ROW 5
#define MAX_COL 9
#define USEC 100000

/* prototypes */
void PutChar( int, int, char ); // show a letter
void CursorHome( void );        // position cursor to top-left
void CursorOff( void );         // hide cursor
void CursorOn( void );          // show (restore) cursor
void ClearScr( void );          // clear the whole screen
void InitScr( void );           // arrange the box
void FlashScr( void );          // flash the screen

void PutChar( int row, int col, char chr )
{
   char str[20];

   sprintf( str, "\033[%d;%dH", row, col ); // move cursor to row and col
   write( OUT, str, strlen(str) );

   write( OUT, &chr, 1 ); // write out chr
}

void CursorHome( void )
{
   write( OUT, "\033[1;1H", 6 );
}

void CursorOff( void )
{
   write( OUT, "\033[?25l", 6 );
}

void CursorOn( void )
{
   write( OUT, "\033[?25h", 6 );
}

void ClearScr( void )
{
   write( OUT, "\033[2J", 4 );
}

void InitScr( void )
{
   int i; 

   CursorOff();

   ClearScr();

   for ( i = 1; i <= MAX_ROW; i++ ) {
      PutChar( i, MAX_COL + 1, '|' ); // write right side of grid
   }

   for ( i = 1; i < MAX_COL + 1; i++ ) {
      PutChar( MAX_ROW + 1, i, '-' ); // write bottom side of grid
   }
}

void FlashScr( void )
{
   char str[20];

   write( OUT, "\033[?5h", 5 ); // screen inverse video mode

   usleep( USEC );

   write( OUT, "\033[?5l", 5 ); // screen normal video mode

   sprintf( str, "\033[%d;%dH", MAX_ROW + 3, 1 ); // move cursor to bottom
   write( OUT, str, strlen(str) );

   CursorOn();
}

int main( int argc, char* argv[] )
{
   int pipes[2];
   char write_pipe_str[8];

   int c_pos[2];
   int r_pos[2];

   sem_t *sem_video, *sem_c, *sem_r;
   char video_key_str[8];
   char c_key_str[8];
   char r_key_str[8];
   pid_t video_key = getpid();

   pid_t pids[2];

   // create pipes for comms  
   if ( -1 == pipe( pipes ) ) {
      perror( "pipe: ");
      exit(1);
   }
   sprintf( write_pipe_str, "%d", pipes[1] );

   sprintf( video_key_str, "%d", video_key );
   sprintf( c_key_str, "c_%d", video_key );
   sprintf( r_key_str, "r_%d", video_key );

   // create semaphores
   sem_video = sem_open( video_key_str, O_CREAT, 0600, 1 );
   sem_c     = sem_open( c_key_str, O_CREAT, 0600, 1 );
   sem_r     = sem_open( r_key_str, O_CREAT, 0600, 1 );

   // check for semaphore creation errors
   if ( sem_video == SEM_FAILED ) {
      perror( "video sem: " );
      exit(1);
   }
   if ( sem_c == SEM_FAILED ) {
      perror( "c sem: " );
      exit(1);
   }
   if ( sem_r == SEM_FAILED ) {
      perror( "r sem: " );
      exit(1);
   }

   // block semaphores so children don't start playing early
   sem_wait( sem_c );
   sem_wait( sem_r );

   // init screen
   sem_wait( sem_video );
   InitScr();
   sem_post( sem_video );

   // create childres (C and R)
   int i;
   for ( i = 0; i < 2; i++ ) {
      pids[i] = fork();
      if ( pids[i] == -1 ) {
         perror( "fork: " );
         exit(1);
      }
      else if ( pids[i] == 0 ) { // in child process
         if ( i == 0 ) { // chaser
            execl( "./child", "child", "C", write_pipe_str, c_key_str, video_key_str, NULL );
         } else if ( i == 1 ) { // runner
            execl( "./child", "child", "R", write_pipe_str, r_key_str, video_key_str, NULL );
         }
      }
   }

   // round robin children until C catches R
   while( 1 ) {

      usleep( USEC );

      sem_post( sem_r );
      read( pipes[0], &r_pos[0], sizeof(int) );
      read( pipes[0], &r_pos[1], sizeof(int) );

      usleep( USEC );

      sem_post( sem_c );
      read( pipes[0], &c_pos[0], sizeof(int) );
      read( pipes[0], &c_pos[1], sizeof(int) );

      if ( c_pos[0] == r_pos[0] && c_pos[1] == r_pos[1] ) {

         // Chaser caught Runner
         for ( i = 0; i < 2; i++ ) {
            kill( pids[i], SIGTERM );
         }

         FlashScr();

         printf( "*** Chaser got Runner! ***\n" );
         printf( "row # %d == %d\n", c_pos[0], r_pos[0] );
         printf( "col # %d == %d\n", c_pos[1], r_pos[1] );
         break;

      }

   }
 
   // cleanup
   close( pipes[0] );
   close( pipes[1] );
   sem_unlink( video_key_str );
   sem_unlink( c_key_str );
   sem_unlink( r_key_str );

   return 0;
}

