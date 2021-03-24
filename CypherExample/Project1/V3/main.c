#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "CreateCypher.h"
#include "FileToString.h"
#include "Encode.h"

/* Return codes. */
enum return_code
  {
   RETURN_SUCCESS,
   RETURN_BAD_USAGE,
   RETURN_FILE_READ,
   RETURN_BAD_CYPHER_FILE
  };

/* Normal usage() method */
void usage(const char* exeName)
{
  printf("Usage: %s <cypher filename> <input filename>\n", exeName);
}

/* main() */
int main(int argc, char* argv[])
{
  /* The input we'll give the encoder. */
  char *input_string;
  /* Holds the map from input to output char. */
  char *translation_table;
  /* Will point to encoded string. */
  char *encoded_text;

  /* Check usage. */
  if ( argc != 3 ) {
    usage(argv[0]);
    return RETURN_BAD_USAGE;
  }

  /* Read the cypher file. */
  if ( (translation_table = CreateCypher(argv[1])) == NULL )
    {
      fprintf( stderr, "Couldn't process cypher file '%s'\n", argv[1]);
      return RETURN_BAD_CYPHER_FILE;
    }
  
  /* Initialize the input string from file */
  input_string = FileToString(argv[2]);
  if ( input_string == NULL )
    {
      fprintf( stderr, "Couldn't read file '%s'\n", argv[2]);
      return RETURN_FILE_READ;
    }

  /* Encode the text. */
  encoded_text = encode( input_string, translation_table );

  /* Print translated string. */
  printf("%20s '%s'\n", "Encoding of", input_string);
  if ( encoded_text )
    printf("%20s '%s'\n", "is", encoded_text );
  else
    printf("encountered an error\n");

  /* free all malloc'ed space. */
  free(translation_table);
  free( encoded_text );
  free(input_string);

  return RETURN_SUCCESS;
}
