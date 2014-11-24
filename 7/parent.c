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

int qid;
pid_t keypoll_pid;
char player = '<';

int main( int argc, char* argv[] )
{

    char character; // current character
    char str[40]; // general purpose string
    pid_t pids[26];
    char qid_str[10];

    msg_t msg;

    atexit( exit_handler );
    signal( SIGCHLD, child_handler );

    sprintf( video_key_str, "%d", getpid() );

    // create video semaphore
    sem_video = sem_open( video_key_str, O_CREAT, 0666, 1 );
    if ( sem_video == SEM_FAILED ) {
        perror( "semaphore" );
        exit(1);
    }

    // request message queue using pid as key
    if ( -1 == ( qid = msgget( getpid(), 0666 | IPC_CREAT ) ) ) {
        perror( "msgget" );
        exit(1);
    }
    
    // convert qid into string so we can pass to children
    sprintf( qid_str, "%d", qid );
    
    ClearScr( sem_video );
    // do keypoll
    keypoll_pid = fork();
    if ( keypoll_pid == -1 ) {
        perror( "fork" );
        exit(1);
    }
    else if ( keypoll_pid == 0 ) {
        execl( "./keypoll", "keypoll", qid_str, NULL );
    }

    // send semaphore in msg to keypoll
    msg.recipient = keypoll_pid;
    msg.character = ' ';
    msg.row = -1;
    strcpy( msg.sem_video_str, video_key_str );
    if ( -1 == msgsnd( qid, (void *)&msg, MSG_SIZE, 0 ) ) {
        perror( "msgsnd" );
        exit(1);
    }

    while( player == '<' ) {
        usleep( USEC );
    }

    InitScr( sem_video );

    sem_wait( sem_video );

    // wish player good luck
    sprintf( str, "\033[%d;%dH", (player - 64), 62 );
    write( OUT, str, strlen(str) );
    sprintf( str, "<-- GOOD LUCK !!" );
    write( OUT, str, strlen(str) );

    sem_post( sem_video );

    int i = 0;
    for ( character = 'A'; character <= 'Z'; character++ ) {
        pids[i] = fork();
        if ( pids[i] == -1 ) {
            perror( "fork" );
            exit(1);
        }
        else if ( pids[i] == 0 ) { // in child process
            execl( "./child", "child", qid_str, NULL );
        }
        i++;
    }

    i = 0;
    for ( character = 'A'; character <= 'Z'; character++ ) {
        // prep message to send
        msg.recipient     = pids[i];//(long int) pids[i];
        msg.character     = character;
        msg.row           = ((int)character - 64);
        strcpy( msg.sem_video_str, video_key_str );

        if ( -1 == msgsnd( qid, (void *)&msg, MSG_SIZE, 0 ) ) {
            perror( "msgsnd" );
            exit(1);
        }
        i++;
        usleep( 10 );
    }

    while ( 1 ) pause();

    return 0; // normal return

}

void child_handler()
{

    pid_t pid;
    char ch;
    int exit_code;
    char str[40]; // general purpose string

    pid = wait( &exit_code );

    // convert exit code back into character
    exit_code <<= 16; exit_code >>= 24; ch = (char)exit_code;

    if ( pid == keypoll_pid ) {
        player = ch;
        return;
    }

    int i = 0;
    if ( i < 5 ) {
        sem_wait( sem_video );

        sprintf( str, "\033[%d;%dH",(ch - 64) ,62 );
        write( OUT, str, strlen(str) );
        sprintf( str, "<-- %d", finishing_place );
        write( OUT, str, strlen(str) );

        sem_post( sem_video );
        i++;
    }

    if ( ch == player ) {
        sem_wait( sem_video );

        sprintf( str, "\033[%d;%dH",(ch - 64) ,62 );
        write( OUT, str, strlen(str) );
        if ( finishing_place == 1 ) {
            sprintf( str, "<-- %d YOU WIN !!", finishing_place );
        } else {
            sprintf( str, "<-- %d YOU LOSE !!", finishing_place );
        }
        write( OUT, str, strlen(str) );

        sem_post( sem_video );
    } 

    printf( "%d-%c ", finishing_place, ch ); // buffered for now
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
    if ( -1 == msgctl( qid, IPC_RMID, 0 ) ) {
        perror( "msgctl" );
    }

}

