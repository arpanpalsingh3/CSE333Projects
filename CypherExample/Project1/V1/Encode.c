#include <stdlib.h>
#include <string.h>

#include "Encode.h"

/* We need a single type cast in the code below.
   We use typedef to define the type to cast into.
   It's a bit of overkill, but illustrates typedef. */
typedef unsigned char BYTE;

/* returns either a malloc()'ed string holding the encoded input text
   or NULL if some error */

char* encode( const char *input_string, const char *cypher_vector )
{
  char* encoded_text;  // return value, if everything goes well
  char* next_ptr;      // used to iterate over output string
  
  /* check input for common caller errors */
  if ( input_string == NULL ||  cypher_vector == NULL )
    return NULL;

  /* allocate space for output string */
  size_t length = strlen(input_string);
  encoded_text = (char*)malloc(length+1);
  if ( encoded_text != NULL )
    {
      /* do the substitution.
	 Note: C strings are terminated by a '\0' character */
      for (next_ptr = encoded_text; *input_string != '\0'; input_string++, next_ptr++ )
	{
	  *next_ptr = cypher_vector[*(BYTE*)input_string];
	}
      /* put the string terminator character on the output string */
      *next_ptr = '\0';
    }
  return encoded_text;
}
