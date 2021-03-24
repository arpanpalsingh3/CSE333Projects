#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileToString.h"

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
  if ( in_file != NULL )
    {
      /* Now we have a problem... we need to allocate space to hold the
	 characters we'll read from the file, but how much space? 
	 +1 for '\0' at end. */
    result_string = (char*)malloc( MAX_FILE_SIZE + 1 );
    
    /* Did malloc succeed? */
    if ( result_string != NULL )
      {
	/* Number of characters read so far. */
	uint32_t count = 0;
	/* Points to next char in output string to write. */
	char* resultstring_ptr = result_string;
	/* Next char from file. */
	char c = fgetc(in_file);
      
	for ( count=0; c != EOF && count < MAX_FILE_SIZE; count++, c = fgetc(in_file) )
	    *resultstring_ptr++ = c;
	*resultstring_ptr = '\0';
      }
    }

  return result_string;
}
