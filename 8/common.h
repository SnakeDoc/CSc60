/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Professor Weide Chang
 *
 * Assignment 8
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
#include <time.h>

#include <sys/time.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <sys/shm.h>

#define OUT fileno(stdout)
#define IN fileno(stdin)

#define MAX_ROW 26    // max number of rows
#define MAX_COL 60    // max number of columns
#define USEC 50000    // base sleep of .05 seconds

typedef struct {
    char alphabet;          // child alphabet
    int row;                // which row to play on
    int col;                // child writes back
    int o_col;              // old child column
    int start;              // start race (0 == false, 1 == true)
    int finishing_place;    // finishing place
    int points;             // points won
    char sem_video_str[20]; // to request access to video semaphore
} info_t;

/* common routine prototypes */
void PutChar( int, int, char, sem_t * );
void ClearScr( sem_t * );
void CursorOff( sem_t * );
void CursorOn( sem_t * );
void FlashScr( sem_t * );
void InitScr( sem_t * );

#endif

