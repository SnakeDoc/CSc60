/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
 *
 */

// server.c

#include "common.h"

/* prototypes */
void child_handler();
void exit_handler();

// global data so we can access in cleanup atexit() routine
char video_key_str[8]; // video semaphore string
int shmid_c;           // shared memory for client
int shmid_c_m;         // shared memory for client monitor
int shmids[26];        // shared memory for alphabets

int main( int argc, char* argv[] )
{

    char character;              // current character
    char str[40];                // general purpose string
    pid_t pids[26];              // child alphabet pids
    info_t *info_c;              // info_t for client
    info_t *info_c_m;            // info_t for client monitor
    info_t* infos[26];           // info_t for all alphabets
    sem_t *sem_video;            // video semaphore
    int finishing_place = 1;     // starting finish place
    char termination_cond = '<'; // the termination condition

    sprintf( video_key_str, "%d", getpid() );

    // create video semaphore
    sem_video = sem_open( video_key_str, O_CREAT, 0666, 1 );
    if ( sem_video == SEM_FAILED ) {
        perror( "server semaphore" );
        exit(1);
    }

    // request/allocate shared memory for client
    if( ( shmid_c = shmget( getpid()+32766, 
                sizeof( info_t ), IPC_CREAT|0666 ) ) < 0 ) {
        perror( "server shmget" );
        exit(1);
    }
    if( ( shmid_c_m = shmget( getpid()+32767, 
                sizeof( info_t ), IPC_CREAT|0666 ) ) < 0 ) {
        perror( "server shmget" );
        exit(1);
    }

    // attach to shared memory for client
    if( ( info_c = (info_t *)shmat( shmid_c, 0, 0 ) ) == (info_t *) -1 ) {
        perror( "server shmat" );
        exit(1);
    }
    if( ( info_c_m = (info_t *)shmat( shmid_c_m, 0, 0 ) ) == (info_t *) -1 ) {
        perror( "server shmat" );
        exit(1);
    }

    // set default alphabet character in client shared memory
    //   we'll use this to notice a change, signaling the client
    //   has made their choice and is ready to race
    info_c->alphabet = '*';
    info_c->row = shmid_c_m; // give the client the monitoring shmid num
    info_c->col = -1;
    info_c->o_col = -1;
    info_c->start = 0;
    info_c->finishing_place = -1;
    info_c->points = -1;
    strcpy( info_c->sem_video_str, video_key_str );

    // more defaults
    info_c_m->alphabet = '*';
    info_c_m->row = -1;
    info_c_m->col = -1;
    info_c_m->o_col = -1;
    info_c_m->start = 0;
    info_c_m->finishing_place = -1;
    info_c_m->points = -1;
    strcpy( info_c_m->sem_video_str, video_key_str );

    ClearScr( sem_video );

    // display client shared memory id so user can
    //   attach to the server
    printf( "From another shell, run client and pick winner.\n" );
    printf( "Syntax: './client %d'\n", shmid_c );

    // now setup the children alphabet process while waiting for client
    int i = 0;
    for ( character = 'A'; character <= 'Z'; character++ ) {
        if( ( shmids[i] = shmget( getpid()+32767+i, 
                sizeof( info_t ), IPC_CREAT|0666 ) ) < 0 ) {
            perror( "server shmget alphabet" );
            exit(1);
        }
        // attach to shared memory
        if ( infos[i] = (info_t *)shmat( shmids[i], 0, 0 ) == (info_t *) -1 ) {
            perror( "server shmat alphabet" );
            exit(1);
        }
        // set defaults
        infos[i]->alphabet = character;
        infos[i]->row = ((int)character - 64);
        infos[i]->col = 1; // default starting column
        infos[i]->o_col = -1; // there is no old column yet
        infos[i]->start = 0;  // don't start the race yet
        infos[i]->finihsing_place = -1; // race has not started yet
        infos[i]->points = -1; // no points yet
        strcpy( infos[i]->sem_video_str, video_key_str );

        i++;
    }
    for ( i = 0; i < 26; i++ ) {
        sprintf( str, "%d", shmid[i] );
        pids[i] = fork();
        if ( pids[i] == -1 ) {
            perror( "server fork alphabet" );
            exit(1);
        }
        else if ( pids[i] == 0 ) { // in child process
            execl( "./alphabet", "alphabet", str, NULL );
        }
        i++;
    }

    // ensure we always cleanup
    atexit( exit_handler );

    while( info_c->alphabet == '*' ) {
        usleep( USEC );
    }

    printf( "\n    >>>>>> Client picked '%c' <<<<<<\n", info_c->alphabet );

    sleep( USEC * 6 );

    ClearScr( sem_video );
    InitScr( sem_video );

    // On your marks!
    sleep( USEC * 3 );
    // Get Set!
    sleep( USEC * 3 );
    // Go!
    for ( i = 0; i < 26; i++ ) {
        infos[i]->start = 1;
    }

    // loop and display children updates
    //   also update client with new positions
    while ( 1 ) {
        int i;
        for ( i = 0; i < 26; i++ ) {
            PutChar( infos[i]->row, infos[i]->o_col, ' ', sem_video );
            PutChar( infos[i]->row, infos[i]->col, 
                            infos[i]->alphabet, sem_video );
            
            // update client monitor
            info_c_m->alphabet = infos[i]->alphabet;
            info_c_m->row = infos[i]->row;
            info_c_m->col = infos[i]->col;
            info_c_m->o_col = infos[i]->o_col;

            if ( infos[i]->col == MAX_COL ) {
                sem_wait( sem_video );
                sprintf( str, "\033[%d;%dH", infos[i]->alphabet - 64, 62 );
                write( OUT, str, strlen(str) );
                sprintf( str, "<-- %d", finishing_place );
                write( OUT, str, strlen(str) );
                if ( finishing_place <= 5 ) {
                    sprintf( str, "\033[%d;%dH", infos[i]->alphabet - 64, 68 );
                    write( OUT, str, strlen(str) );
                    sprintf( str, "%s", "***" );
                    write( OUT, str, strlen(str) );
                }
                sem_post( sem_video );
                infos[i]->finishing_place = finishing_place;
                infos[i]->points = 26 - finishing_place;
                // update client monitor
                info_c_m->finishing_place = infos[i]->finishing_place;
                info_c_m->points = infos[i]->points;
                finishing_place++;
            }
        }
        if ( finishing_place > 26 ) break;
    }

    // inform client race is over
    info_c_m->alphabet = '<';

    // assign client their points
    info_c->points = infos[info_c->alphabet - 64]->points;

    FlashScr( sem_video );
    CursorOn( sem_video );
    printf( "Client points: %d\n", info_c->points );
    if ( points < 21 ) {
        printf( "Better luck next time!\n" );
    } else {
        printf( "Nice!\n" );
    }

    return 0; // normal return

}

void exit_handler()
{

    // cleanup routine
    sem_unlink( video_key_str );
    shmctl( shmid_c, IPC_RMID, 0 );
    shmctl( shmid_c_m, IPC_RMID, 0 );
    int i;
    for ( i = 0; i < 26; i++ ) {
        shmctl( shmids[i], IPC_RMID, 0 );
    }

}

