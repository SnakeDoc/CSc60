/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
 *
 */

// keypoll.c

#include "common.h"

#include <termios.h>

/* prototypes */
char KeyPoll( void );

int main( int argc, char* argv[] )
{

    sem_t *sem;
    char sem_str[20];

    char str[40]; // general purpose string

    strcpy( sem_str, argv[1] );

    sem = sem_open( sem_str, O_CREAT );
    if ( sem == SEM_FAILED ) {
        perror( "keypoll semaphore" );
        exit(1);
    }

    CursorOff( sem );

    sem_wait( sem );

    sprintf( str, "\033[%d;%dH", 1, 1 );
    write( OUT, str, strlen(str) );
    sprintf( str, "Pick a character from the alphabet" );
    write( OUT, str, strlen(str) );

    sprintf( str, "\033[%d;%dH", 2, 2 );
    write( OUT, str, strlen(str) );
    sprintf( str, "5 seconds !!" );
    write( OUT, str, strlen(str) );

    sem_post( sem );

    int i;
    int j = 5;
    char ch;
    // increase keypoll sensitivity -- check 4 times a second
    for ( i = 20; i > 0; i-- ) {
        if ( (i % 5) == 0 ) {
            PutChar( 2, 2, j + 48, sem );
            j--;
        }
        usleep( USEC );
        ch = KeyPoll();
        if ( ch != ' ' && (int)ch > 64 && (int)ch < 123 ) {
            if ( (int)ch > 90 ) {
                ch = (char)((int)ch - 32); // make it a capital letter
            }
            break;
        }
    }

    sprintf( str, "\033[%d;%dH", 2, 2 ); // will position cursor
    if ( ch == ' ' ) {
      // i'll pick for you

        // be more random
        srand( time(NULL) * getpid() );
        ch = (char) (rand() % 26) + 64;

        char sentence[40];
        sprintf( sentence, "0 seconds !! I've picked for you:" );
        sem_wait( sem );

        write( OUT, str, strlen(str) ); // position cursor
        write( OUT, sentence, strlen(sentence) );

        sem_post( sem );

        sprintf( str, "\033[%d;%dH", 2, (strlen(sentence) + 3) );
        char space = ' ';

        sem_wait( sem );
        for ( i = 0; i < 15; i++ ) {
            write( OUT, str, strlen(str) );
            write( OUT, &space, 1 );

            usleep( USEC * 2 );

            write( OUT, str, strlen(str) );
            write( OUT, &ch, 1 );

            usleep( USEC * 2 );
        }
        sem_post( sem );
    } else {
      // you picked
        char sentence[40];
        sprintf( sentence, "0 seconds !! You've picked:" );
        sem_wait( sem );

        write( OUT, str, strlen(str) ); // position cursor
        write( OUT, sentence, strlen(sentence) );

        sem_post( sem );

        sprintf( str, "\033[%d;%dH", 2, (strlen(sentence) + 3) );
        char space = ' ';

        sem_wait( sem );
        for ( i = 0; i < 7; i++ ) {
           
            write( OUT, str, strlen(str) );
            write( OUT, &space, 1 );

            usleep( USEC * 2 );
           
            write( OUT, str, strlen(str) ); 
            write( OUT, &ch, 1 );

            usleep( USEC * 2 );
        }
        sem_post( sem );
    }
    usleep( USEC * 10 );
    exit ((int)ch);
}

char KeyPoll()
{
    struct termios oldt, newt;          // old termio, new termio
    struct timeval my_tv;               // time value
    fd_set my_fds;                      // a set of file descriptors
    char ch;                            // char pressed

    my_tv.tv_sec = 0;                   // no whole seconds
    my_tv.tv_usec = USEC;               // microseconds

    // change mode
    tcgetattr( IN, &oldt );             // save stdin file descriptor
    newt = oldt;                        // new copy to use
    newt.c_lflag &= ~( ICANON | ECHO ); // disable buffering and ECHO
    tcsetattr( IN, TCSANOW, &newt );    // set new term attr of stdin

    // set stdin to be the target
    FD_ZERO( &my_fds );                 // clear them out first
    FD_SET( IN, &my_fds );              // add stdin to set

    // poll stdin, this will change my_fds and my_tv
    // select: 1 channel to poll, a set of read-fds, a set of write-fds,
    // a set of except-fds, and time value
    select( 1, &my_fds, 0, 0, &my_tv );

    // if flag in my_fds set, a key was pressed
    if ( FD_ISSET( IN, &my_fds ) ) {
        ch = getchar(); // get ch from stdin
    } else {
        ch = ' '; // space is nothing pressed
    }

    // change mode back with original termio
    tcsetattr( IN, TCSANOW, &oldt ); // set term attr of stdin back

    return ch;
}
