#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Cypher.h"
#include "FileToString.h"

/* Return codes */
enum return_code {
		  RETURN_SUCCESS,
		  RETURN_BAD_USAGE,
		  RETURN_FILE_OPEN,
		  RETURN_BAD_CYPHER_FILE
};

/* usage() */
void usage(const char* exeName)
{
  printf("Usage: %s <cypher filename> <input filename>\n", exeName);
}

/* main() */
int main(int argc, char* argv[]) {
  /* The input we'll give the encoder. */
  char   *input_string;
  /* Will point to encoded string. */
  char   *encoded_text;
  /* The object that can translate strings. */
  Cypher cypher;

  /* Check usage. */
  if ( argc != 3 )
    {
      usage(argv[0]);
      return RETURN_BAD_USAGE;
    }

  /* Read the translation table */
  if ( (cypher = Cypher_new(argv[1])) == NULL )
    {
      fprintf( stderr, "Couldn't process cypher file '%s'\n", argv[1]);
      return RETURN_BAD_CYPHER_FILE;
    }
  
  /* Initialize he input string. */
  input_string = FileToString(argv[2]);
  if ( input_string == NULL )
    {
      fprintf( stderr, "Couldn't read file '%s'\n", argv[2]);
      return RETURN_FILE_OPEN;
    }

  /* Encode the text. */
  encoded_text = Cypher_encode( cypher, input_string );

  /* Print encoded string. */
  printf("%20s '%s'\n", "Encoding of", input_string);
  if ( encoded_text )
    printf("%20s '%s'\n", "is", encoded_text );
  else
    printf("encountered an error\n");

  /* free malloc'ed space. */
  free( encoded_text );
  free(input_string);

  /* Give cypher object chance to clean up. */
  Cypher_destroy( cypher );

  return EXIT_SUCCESS;
}
