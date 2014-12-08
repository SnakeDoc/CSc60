 #
 # Jason Sipula
 # California State University Sacramento
 # CSc 60 - Introduction to System Programming in UNIX
 # Professor Weide Chang
 #
 # Assignment 8
 #
 # Makefile for Assignment 8

CMD_DELETE = /bin/rm -f

CC = gcc
MODEL = -m32

CFLAGS = $(MODEL) -Wall    # *.c, and compiler flags
LFLAGS = $(MODEL)          # *.o, and linker flags
LIBS = -lrt                # linking real-time library

# ----------------------------------------------------

all: keypoll alphabet client server
	$(CMD_DELETE) *.o
	chmod 755 keypoll alphabet client server

clean:
	$(CMD_DELETE) *.o keypoll alphabet client server *~ #*#

# ----------------------------------------------------

server: server.o common.o
	$(CC) $(LFLAGS) -o $@ server.o common.o $(LIBS)

client: client.o common.o
	$(CC) $(LFLAGS) -o $@ client.o common.o $(LIBS)

alphabet: alphabet.o common.o
	$(CC) $(LFLAGS) -o $@ alphabet.o common.o $(LIBS)

keypoll: keypoll.o common.o
	$(CC) $(LFLAGS) -o $@ keypoll.o common.o $(LIBS)

