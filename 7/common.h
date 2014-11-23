/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 7
 *
 */

// common.h

#ifndef __COMMON_H__
#define __COMMON_H__

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>

#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define OUT fileno(stdout)
#define IN fileno(stdin)

#define MAX_ROW 26    // max number of rows
#define MAX_COL 60    // max number of columns
#define USEC 50000    // base sleep of .05 seconds

typedef struct {
    long int recipient;     // whom msg is for
    char character;         // child character
    int row;                // which row to play on
    char sem_video_str[20]; // to request access to video semaphore
} msg_t;

#define MSG_SIZE ( sizeof( msg_t ) - sizeof( long int ) )

/* common routine prototypes */
void PutChar( int, int, char, *sem_t);
void ClearScr( *sem_t );
void CursorOff( *sem_t );
void CursorOn( *sem_t );
void FlashScr( *sem_t );
void InitScr( *sem_t );

#endif

