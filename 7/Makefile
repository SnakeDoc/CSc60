 #
 # Jason Sipula
 # California State University Sacramento
 # CSc 60 - Introduction to System Programming in UNIX
 # Professor Weide Chang
 #
 # Assignment 7
 #
 # Makefile for Assignment 7

CMD_DELETE = /bin/rm -f

CC = gcc
MODEL = -m32

CFLAGS = $(MODEL) -Wall    # *.c, and compiler flags
LFLAGS = $(MODEL)          # *.o, and linker flags
LIBS = -lrt                # linking real-time library

# ----------------------------------------------------

all: keypoll parent child
	$(CMD_DELETE) *.o
	chmod 755 keypoll parent child

clean:
	$(CMD_DELETE) *.o keypoll parent child *~ #*#

# ----------------------------------------------------

parent: parent.o common.o
	$(CC) $(LFLAGS) -o $@ parent.o common.o $(LIBS)

child: child.o common.o
	$(CC) $(LFLAGS) -o $@ child.o common.o $(LIBS)

keypoll: keypoll.o common.o
	$(CC) $(LFLAGS) -o $@ keypoll.o common.o $(LIBS)

