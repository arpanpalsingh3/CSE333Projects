#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "Cypher.h"

/* Define the Cypher_t struct that a Cypher points to. 
   We do it here, in the .c, so that it's visible only
   in this .c file, and not available to client code. */
struct Cypher_t {
  char trans_table[256];
};

typedef unsigned char BYTE;

/* Create cypher map from description in file.
   File has pairs of non-space characters indicating a
   encoding of the first of the pair as the second. */
Cypher Cypher_new(const char* filename)
{
  /* "this" has no special meaning in C. */
  Cypher this = NULL;
  FILE* in_file;
  
  if ( filename != NULL && (in_file = fopen(filename, "r")) != NULL )
    {
      /* Allocate and initialize "this object" */
      this = (Cypher)malloc(sizeof(*this));
      /* malloc failed? */
      if ( this == NULL )
	return NULL;
    
      /* Set default translation values. */
      for ( int i=0; i<256; i++ )
	this->trans_table[i] = i;

      /* Read each successive non-blank string from the file and process.
	 It should be a pair of characters, meaning translate from the first
	 to the second.
	 We do not detect duplicate translations for an individual character. */
      char *nextPair;
      while ( fscanf(in_file, "%ms", &nextPair)==1 )
	{
	  if ( strlen(nextPair) != 2 )
	    {
	      fprintf(stderr, "Error: bad cypher file entry: '%s'\n", nextPair);
	      free(nextPair);
	      free(this);
	      this = NULL;
	      break;
	    }
	  this->trans_table[(BYTE)nextPair[0]] = nextPair[1];
	  free(nextPair);
	}
    }

  return this;
}

/* destroy() method. */
void Cypher_destroy(Cypher this)
{
  /* Free() accepts null as an argument, but just in case we check... */
  if ( this != NULL )
    free(this);
}

/* encode() method.
   Returns either a malloc()'ed string holding the encoded input text
   or NULL if some error. */
char* Cypher_encode( Cypher this, const char *input_string )
{
  /* Return value, if everything goes well. */
  char* encoded_text;
  /* Used to iterate over output string. */
  char* next_ptr;
  
  /* Check input for common caller errors. */
  if ( input_string == NULL ||  this == NULL )
    return NULL;

  /* Allocate space for output string. */
  /* We need one more than  input string length because of '\0' terminator. */
  size_t length = strlen(input_string);
  encoded_text = (char*)malloc(length+1);
  if ( encoded_text != NULL )
    {
      /* Do the substitution.
	 Note: C strings are terminated by a '\0' character. */
      for (next_ptr = encoded_text; *input_string != '\0'; input_string++, next_ptr++ )
	{
	  *next_ptr = this->trans_table[*(BYTE*)input_string];
	}
      /* Put the string terminator character on the output string. */
      *next_ptr = '\0';
    }
  
  return encoded_text;
}
