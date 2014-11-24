/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 7
 *
 */

// parent.c

#include "common.h"

/* prototypes */
void child_handler();
void exit_handler();

sem_t *sem_video;
char video_key_str[8];
int finishing_place = 1;

int main( int argc, char* argv[] )
{

    char character, str[20]; // current character, and general purpose str
    int qid, pid;
    char qid_str[10];

    msg_t msg;

    atexit( exit_handler );
    signal( SIGCHLD, child_handler );

    sprintf( video_key_str, "%d", getpid() );

    // create video semaphore
    sem_video = sem_open( video_key_str, O_CREAT, 0600, 1 );
    if ( sem_video == SEM_FAILED ) {
        perror( "semaphore: " );
        exit(1);
    }

    // request message queue using pid as key
    if ( -1 == ( qid = msgget( getpid(), 0600 | IPC_CREAT ) ) ) {
        perror( "msgget: " );
        exit(1);
    }

    // convert qid into string so we can pass to children
    sprintf( qid_str, "%d", qid );

    InitScr( sem_video );

    for ( character = 'A'; character <= 'Z'; character++ ) {
        pid = fork();
        if ( pid == -1 ) {
            perror( "fork: " );
            exit(1);
        }
        else if ( pid == 0 ) { // in child process
            execl( "./child", "child", qid_str, NULL );

            // prep message to send
            msg.recipient     = (long int) pid;
            msg.character     = character;
            msg.row           = ((int)character - 64);
            strcpy( msg.sem_video_str, video_key_str );

            if ( -1 == msgsnd( qid, (void *)&msg, MSG_SIZE, 0 ) ) {
                perror( "msgsnd: " );
                exit(1);
            }
        }
    }

    return 0; // normal return

}

void child_handler()
{

    pid_t pid;
    char ch;
    int exit_code;

    pid = wait( &exit_code );

    // convert exit code back into character
    exit_code <<= 16; exit_code >>= 24; ch = (char)exit_code;

    printf( "%d-%c", finishing_place, ch ); // buffered for now
    finishing_place++;

    if ( finishing_place > 26 ) {
        FlashScr( sem_video );
        CursorOn( sem_video );
        printf( "\n" ); // release buffer

        exit(0);
    }

}

void exit_handler()
{

    // cleanup routine
    sem_unlink( video_key_str );

}

