/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
 *
 */

// alphabet.c

#include "common.h"

int main( int argc, char* argv[] )
{

    info_t *my_info;
    char *p;
    int shmid, i;

    // attach to shared memory
    shmid = atoi( argv[1] );
    if ( p = shmat( shmid, 0, 0 ) == (char *) -1 ) {
        perror( "alphabet shmat" );
        exit(1);
    }
    my_info = (info_t *)p;

    // be more random
    srand( time(NULL) * getpid() );

    while ( my_info->start == 0 ) sleep( USEC );

    for ( i = 1; i <= MAX_COL; i++ ) {
        usleep( USEC * ( rand() % 4 ) );
        my_info->o_col = my_info->col;
        my_info->col = i;
    }

    exit(my_info->row); // don't return 0 since atexit() 
                        //    on server would catch it
}
