#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileToString.h"

/* This code cannot handle correctly files larger than this! */
#define MAX_FILE_SIZE 10000

/* Returns a string with the first up to MAX_FILE_SIZE characters of
   the file or NULL for error. */
char* FileToString(const char* filename)
{
  /* Will hold file contents. */
  char* result_string = NULL;

  /* Try to open file for reading. */
  FILE * in_file = fopen(filename, "r");

  /* Were we able to open the file? */
  /* Note: It's not uncommon to see C programmers write this as
     if ( in_file )
     In C, anything thatg evaluates to 0 is false and anthing
     that evaluates to non-zero is true.  NULL is 0, so in
     C the logical expression "in_file != NULL" and "in_file" 
     are the same.  */
  if ( in_file != NULL )
    {
      /* Now we have a problem... we need to allocate space to hold the
	 characters we'll read from the file, but how much space? */
      result_string = (char*)malloc( MAX_FILE_SIZE + 1 ); 
    
      /* Did malloc succeed? */
      if ( result_string != NULL )
	{
	  /* Next char from file */
	  char c = fgetc(in_file);
	  /* Number of characters read so far. */
	  uint32_t count = 0;
	  /* Points to next char in output string to write. */
	  char* resultstring_ptr = result_string;
      
	  /* need to reserve a byte for the terminating '\0' */
	  for ( count=0; c != EOF && count < MAX_FILE_SIZE; count++ )
	    {
	      *resultstring_ptr++ = c;
	      c = fgetc(in_file); 
	    }
	  *resultstring_ptr = '\0';
	}
    }

  return result_string;
}
