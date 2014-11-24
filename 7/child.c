/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 7
 *
 */

// child.c

#include "common.h"

int main( int argc, char* argv[] )
{

    msg_t msg;
    int qid, row, i;
    char character;
    sem_t *sem_video;

    qid = atoi( argv[1] );
    
    if ( -1 == msgrcv( qid, (void *)&msg, MSG_SIZE, getpid(), 0 ) ) {
        perror( "msgrcv" );
        exit(1);
    }

    character = msg.character;
    row = msg.row;
    
    sem_video = sem_open( msg.sem_video_str, O_CREAT );
    if ( sem_video == SEM_FAILED ) {
        perror( "child semaphore" );
        exit(1);
    }

    // be more random
    srand( time(NULL) * getpid() );

    for ( i = 1; i <= MAX_COL; i++ ) {
        PutChar( row, i, character, sem_video );
        usleep( USEC * ( rand() % 4 ) );
        if ( i == MAX_COL ) break;
        PutChar( row, i, ' ', sem_video );
    }

    exit( (int)character );
}
