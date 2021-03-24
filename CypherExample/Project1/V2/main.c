#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "FileToString.h"
#include "Encode.h"

/* Return codes definitions.
   An enum in C creates symbolic names for int's.  By default, the compiler assigns
   0 to the first name, and then increments by 1 for each successive name. */
enum return_code
  {
   RETURN_SUCCESS,
   RETURN_BAD_USAGE,
   RETURN_FILE_READ
  };

void usage(const char* exeName)
{
  printf("Usage: %s <filename>\n", exeName);
}

int main(int argc, char* argv[])
{
  /* The input we'll give the encoder. */
  char *input_string;
  /* Holds the map from input to output char. */
  char cypher[256];
  /* Will point to encoded string. */
  char *encoded_text;

  if ( argc != 2 )
    {
      usage(argv[0]);
      return RETURN_BAD_USAGE;
    }
  
  /* Initialize input string. */
  input_string = FileToString(argv[1]);
  if ( input_string == NULL )
    {
      fprintf( stderr, "Couldn't read file '%s'\n", argv[1]);
      return RETURN_FILE_READ;
    }

  /* Establish the substitution map.
     Our cypher just converts 'a' to 'Z' and leaves everything else unchanged. */
  for (int i=0; i<256; i++)
    cypher[i] = i;
  cypher['a'] = 'Z';

  /* Encode the text. */
  encoded_text = encode( input_string, cypher );

  /* Print the encoded string. */
  printf("%20s '%s'\n", "Encoding of", input_string);
  if ( encoded_text )
    printf("%20s '%s'\n", "is", encoded_text );
  else
    printf("encountered an error\n");

  /* free malloc'ed space. */
  free( encoded_text );
  free(input_string);

  return RETURN_SUCCESS;
}
