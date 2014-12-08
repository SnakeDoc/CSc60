/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
 *
 */

// client.c

#include "common.h"

int main( int argc, char* argv[] )
{

    int shmid, keypoll_pid, exit_code;
    char *p;
    char *p2;
    info_t *my_info;
    info_t *their_info;
    char player;
    sem_t *sem_video;

    if ( argc != 2 ) {
        printf( "Invalid invocation! Syntax: client <shared memory id>\n" );
        exit(1);
    }

    // attach to shared memory
    shmid = atoi( argv[1] );
    if ( ( p = shmat( shmid, 0, 0 ) ) == (char *) -1 ) {
        perror( "client shmat" );
        exit(1);
    }
    my_info = (info_t *)p;

    // attach to monitor shared memory
    //   the shmid for the monitor is stored in the row by default
    if ( ( p2 = shmat( my_info->row, 0, 0 ) ) == (char *) -1 ) {
        perror( "client shmat" );
        exit(1);
    }
    their_info = (info_t *)p2;

    // get a local copy of the video semaphore
    sem_video = sem_open( my_info->sem_video_str, O_CREAT );
    if ( sem_video == SEM_FAILED ) {
        perror( "client video semaphore" );
        exit(1);
    }

    ClearScr( sem_video );

    // do keypoll
    keypoll_pid = fork();
    if ( keypoll_pid == -1 ) {
        perror( "client keypoll fork" );
        exit(1);
    }
    else if ( keypoll_pid == 0 ) {
        execl( "./keypoll", "keypoll", my_info->sem_video_str, NULL );
    }

    // wait for keypoll to exit
    wait( &exit_code );
    // get the keypoll character
    exit_code <<= 16; exit_code >>= 24; player = (char)exit_code;

    // initialize local gameboard
    InitScr( sem_video );

    // wish player good luck
    sem_wait( sem_video );
    sprintf( str, "\033[%d;%dH", (player - 64), 62 );
    write( OUT, str, strlen(str) );
    sprintf( str, "<-- GOOD LUCK !!" );
    write( OUT, str, strlen(str) );
    sem_post( sem_video );

    // inform server of player choice
    my_info->alphabet = player;

    // character '<' is the termination condition
    //   signifying the race has concluded and we
    //   can stop looping
    while( their_info->alphabet != '<' ) {
        PutChar( their_info->row, their_info->o_col, ' ', sem_video );
        PutChar( their_info->row, their_info->col, 
                        their_info->alphabet, sem_video );
        if ( their_info->col == MAX_COL ) {
            sem_wait( sem_video );
            sprintf( str, "\033[%d;%dH", their_info->alphabet - 64, 62 );
            write( OUT, str, strlen(str) );
            sprintf( str, "<-- %d", their_info->finishing_place );
            write( OUT, str, strlen(str) );
            if ( their_info->finishing_place <= 5 ) {
                sprintf( str, "\033[%d;%dH", their_info->alphabet - 64, 68 );
                write( OUT, str, strlen(str) );
                sprintf( str, "%s", "***" );
                write( OUT, str, strlen(str) );
            }
            sem_post( sem_video );
        }

    }

    // did we win any points?
    printf( " Points won: %d", my_info->points );

    FlashScr( sem_video );
    CursorOn( sem_video );
    printf( "\n" ); // release buffer

    if( my_info->row < 21 ) {
        printf( "Better luck next time!\n" );
    } else {
        printf( "Nice!\n" );
    }

    exit(0);
}

