/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
 *
 */

// common.c

#include "common.h"

void PutChar( int row, int col, char chr, sem_t *sem )
{
    char str[20];
    sprintf( str, "\033[%d;%dH", row, col ); // will move cursor to row/col

    sem_wait( sem );

    write( OUT, str, strlen(str) ); // move cursor to position
    write( OUT, &chr, 1 ); // write out chr

    sem_post( sem );
}

void ClearScr( sem_t *sem )
{
    sem_wait( sem );

    write( OUT, "\033[2J", 4 );

    sem_post( sem );
}

void CursorOff( sem_t *sem )
{
    sem_wait( sem );

    write( OUT, "\033[?25l", 6 );

    sem_post( sem );
}

void CursorOn( sem_t *sem )
{
    sem_wait( sem );

    write( OUT, "\033[?25h", 6 );

    sem_post( sem );
}

void FlashScr( sem_t *sem )
{
    char str[20];
    sprintf( str, "\033[%d;%dH", MAX_ROW + 2, 1 ); // will move cursor

    sem_wait( sem );

    write( OUT, "\033[?5h", 5 ); // inverse screen mode
    usleep( USEC );
    write( OUT, "\033[?5l", 5 ); // normal screen mode

    write( OUT, str, strlen(str) ); // move cursor to bottom

    sem_post( sem );
}

void InitScr( sem_t *sem )
{
    int i;
    char ch;

    CursorOff( sem );
    ClearScr( sem );

    for ( i = 1; i < MAX_ROW + 1; i++ ) {
        PutChar( i, MAX_COL, '.', sem );
    }

    i = 1;
    for ( ch = 'A'; ch <= 'Z'; ch++ ) {
        PutChar( i, 1, ch, sem );
        i++;
    }

}

