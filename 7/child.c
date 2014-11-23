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
    char sem_video_str[20];
    sem_t *sem_video;

    qid = atoi( argv[1] );

    if ( -1 == msgrcv( qid, (void *)&msg, MSG_SIZE, getpid(), 0 ) ) {
        perror( "msgrcv: " );
        exit(1);
    }

    character = msg.character;
    sem_video_str = msg.sem_video_str;
    row = msg.row;

    sem_video = sem_open( sem_video_str, O_CREAT );
    if ( sem_video == SEM_FAILED ) {
        perror( "semaphore: " );
        exit(1);
    }

    srand( getpid() );

    for ( i = 1; i <= MAX_COL; i++ ) {
        PutChar( row, i, character, sem_video );
        usleep( USEC * ( rand() % 4 ) );
        if ( i == MAX_COL ) break;
        PurChar( row, i, ' ', sem_video );
    }

    exit (int)character;
}
