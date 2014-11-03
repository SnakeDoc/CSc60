/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 6
 *
 */

// child.c

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <semaphore.h>

#define _REENTRANT
#define IN fileno(stdin)
#define OUT fileno(stdout)
#define MAX_ROW 5
#define MAX_COL 9
#define USEC 100000

/* prototypes */
void PutChar( int, int, char ); // show a letter

void PutChar( int row, int col, char chr )
{
   char str[20];

   sprintf( str, "\033[%d;%dH", row, col ); // move cursor to row and col
   write( OUT, str, strlen(str) );

   write( OUT, &chr, 1 ); // write out chr
}

int main( int argc, char* argv[] )
{
   char CHAR;
   int write_pipe;
   char sem_key[8];
   char video_sem_key[8];

   int disp_row, disp_col;
   int row, col;
   int spaces_move;

   CHAR = argv[1][0];
   write_pipe = atoi( argv[2] );
   strcpy( sem_key, argv[3] );
   strcpy( video_sem_key, argv[4] );

   // open shared semaphores
   sem_t *sem = sem_open( sem_key, O_CREAT );
   sem_t *video_sem = sem_open( video_sem_key, O_CREAT );

   // check for semaphore errors
   if ( sem == SEM_FAILED ) {
      perror( "sem: " );
      exit(1);
   }
   if ( video_sem == SEM_FAILED ) {
      perror( "video_sem: " );
      exit(1);
   }

   if ( CHAR == 'C' ) { // we are chaser
      row = 1;
      col = 1;
      disp_row = 3;
      disp_col = 40;
   } else if ( CHAR == 'R' ) { // we are runner
      row = MAX_ROW;
      col = MAX_COL;
      disp_row = 1;
      disp_col = 40;
   }

   // display starting coordinates and character (C or R)
   sem_wait( video_sem );
   PutChar( row, col, CHAR );
   PutChar( disp_row, disp_col, row + 48 );
   PutChar( disp_row, disp_col + 2, col + 48 );
   sem_post( video_sem );

   // seed random gen
   srand( getpid() );

   // play when it's our turn to go
   while( 1 ) {

      sem_wait( sem );

      sem_wait( video_sem );

      // blank current position
      PutChar( row, col, ' ' );

      // get new position
      spaces_move = rand() % 3 + (-1);
      if ( (spaces_move + row) <= MAX_ROW && (spaces_move + row) > 0) {
         row += spaces_move;
      }
      spaces_move = rand() %3 + (-1);
      if ( (spaces_move + col) <= MAX_COL && (spaces_move + col) > 0) {
         col += spaces_move;
      }

      // display new position
      PutChar( row, col, CHAR );

      PutChar( disp_row, disp_col, row + 48 );
      PutChar( disp_row, disp_col + 2, col + 48 );

      sem_post( video_sem );

      // inform parent of our new position
      write( write_pipe, &row, sizeof(int) );
      write( write_pipe, &col, sizeof(int) );

      usleep( USEC + (rand() % 3) );

   }

   return 0;
}

