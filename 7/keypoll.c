/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 7
 *
 */

// keypoll.c

#include "common.h"

int main( int argc, char* argv[] )
{

    printf( "Pick a character from the alphabet\n" );
    printf( " 5 seconds !!" );
    fflush(OUT);

    int i;
    char ch;
    for ( i = 5;, i > 0; i-- ) {
        PutChar( 2, 2, i + 48, sem );
        sleep(1);
        ch = KeyPoll();
        if ( ch != ' ' && (int)ch > 64 && (int)ch < 123 ) {
            if ( (int)ch > 90 ) {
                ch = (char)((int)ch - 32); // make it a capital letter
            }
            break;
        }
    }

    char str[20];
    sprintf( str, "\033[%d;%dH", 2, 2 ); // will position cursor
    if ( ch == ' ' ) {
      // i'll pick for you

        srand( getpid() );
        ch = (char) (rand() % 26) + 64;

        char sentence[20];
        sprintf( sentence, " 0 seconds !! I've picked for you:  " );
        sem_wait( sem );

        write( OUT, str, strlen(str) ); // position cursor
        write( OUT, sentence, strlen(sentence) );

        sem_post( sem );
        for ( i = 0; i < 5; i++ ) {
            PutChar( 2, strlen(sentence), ch, sem );
            usleep( USEC );
            PutChar( 2, strlen(sentence), ' ', sem );
        }
    } else {
      // you picked
        char sentence[20];
        sprintf( sentence, " 0 seconds !! You've picked:  " );
        sem_wait( sem );

        write( OUT, str, strlen(str) ); // position cursor
        write( OUT, sentence, strlen(sentence) );

        sem_post( sem );
        for ( i = 0; i < 5; i++ ) {
            PutChar( 2, strlen(sentence), ch, sem );
            usleep( USEC );
            PutChar( 2, strlen(sentence), ' ', sem );
        }
    }
    return (int)ch;
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
    select( 1, &my_fds, IN, IN, &my_tv );

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
