/*
 * Jason Sipula
 * California State University Sacramento
 * CSc 60 - Introduction to System Programming in UNIX
 * Assignment #2
 *
 */

#include <stdio.h>
#include <string.h>

#define OUT fileno(stdout)
#define IN fileno(stdin)
#define ASCII_MASK 48

char name[20];
int nameLen;

/* prototypes */
void partA( char* );
void partB( char* );
void partC( char* );
void displayMsg( char* );
void displayChar( char );
void readName( void );

int main( int argc, char* argv[] )
{

   displayMsg("Enter your last name: ");
   readName();
   displayChar('\n');
   
   partA(name);
   displayChar('\n');

   partB(name);
   displayChar('\n');

   partC(name);
   displayChar('\n');

   return 0; // normal return

}

// Part A
void partA( char* msg )
{

   int array[8] = {0,0,0,0,0,0,0,0}; // binary array for char

   int i, j; // loop counters
   for (i = 0; i < strlen(msg); i++)
   {
      displayChar(msg[i]);
      displayChar(' ');
      
      // get binary representation and put into array
      for (j = 0; j < 8; ++j)
      {
         array[j] = (msg[i] >> j) & 1;
      }

      // now print the array
      for (j = 8; j >= 0; j--)
      {
         displayChar((char)(array[j] + ASCII_MASK));
         displayChar(' ');
      }
      // new line
      displayChar('\n');

   }

}

// Part B
void partB( char* msg )
{

   char firstLetter = msg[0];
   int array[8] = {0,0,0,0,0,0,0,0}; // binary array for char

   int i, j; // loop counters
   for (i = 0; i < strlen(msg); i++)
   {
      displayChar(msg[i]);
      displayChar(' ');

      // get binary representation and put into array
      // (this time make sure to | with first letter)
      for (j = 0; j < 8; ++j)
      {
         array[j] = ((msg[i] | firstLetter) >> j) & 1;
      }
      
      // now print the array
      for (j = 8; j >= 0; j--)
      {
         displayChar((char)(array[j] + ASCII_MASK));
         displayChar(' ');
      }
      // new line
      displayChar('\n');
      
   }

}

// Part C
void partC( char* msg )
{

   char firstLetter = msg[0];
   int array[8] = {0,0,0,0,0,0,0,0}; // binary array for char

   int i, j; // loop counters
   for (i = 0; i < strlen(msg); i++)
   {
      displayChar(msg[i]);
      displayChar(' ');

      // get binary representation and put into array
      // (this time, make sure to & with first letter)
      for (j = 0; j < 8; ++j)
      {
         array[j] = ((msg[i] & firstLetter ) >> j) & 1;
      }
      
      // now print the array
      for (j = 8; j >= 0; j--)
      {
         displayChar((char)(array[j] + ASCII_MASK));
         displayChar(' ');
      }
      // new line
      displayChar('\n');
      
   } 

}

void displayMsg( char* msg )
{

   write (OUT, msg, strlen(msg));

}

void displayChar( char c )
{

   write (OUT, &c, 1);

}

void readName( void )
{

   nameLen = read (IN, name, 20);
   nameLen--;
   name[nameLen] = 0;

}

