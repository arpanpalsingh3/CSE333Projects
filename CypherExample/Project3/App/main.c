#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Cypher.h"

/* Return codes. */
enum return_code {
		  RETURN_SUCCESS,
		  RETURN_BAD_USAGE,
		  RETURN_FILE_OPEN,
		  RETURN_BAD_CYPHER_FILE,
		  RETURN_ENCODING_ERROR
};

/* Size of each encoding chunk */
#define CHUNKSIZE 1024

/* usage() method */
void usage(const char* exeName)
{
  printf("Usage: %s <cypher filename>\nReads stdin, writes stdout", exeName);
}

/* main() */
int main(int argc, char* argv[])
{
  /* Will point to encoded string. */
  char   *encoded_text;
  /* The object that can translate strings. */
  Cypher cypher;

  if ( argc != 2 )
    {
      usage(argv[0]);
      return RETURN_BAD_USAGE;
    }

  /* Create the cypher object. */
  if ( (cypher = Cypher_new(argv[1])) == NULL )
    {
      fprintf( stderr, "Couldn't process cypher file '%s'\n", argv[1]);
      return RETURN_BAD_CYPHER_FILE;
    }
  
  /* Read and encode the text a chunk at a time SO THAT we don't have
     to bring entire contents of file into memory at once to encode it. */
  char buf[CHUNKSIZE+1];
  int nRead;
  while ( (nRead = fread(buf, sizeof(char), CHUNKSIZE, stdin)) )
    {

      /* fread() fills an array of bytes.  We need to convert that to a
	 C string, so allocate an extra byte for the '\0' string terminator. */
      buf[nRead] = '\0';
      encoded_text = Cypher_encode( cypher, buf );
      if ( encoded_text == NULL )
	      return RETURN_ENCODING_ERROR;
      printf("%s", encoded_text);
      free( encoded_text );
    }

  /* Done translating, give cypher object chance to clean up. */
  Cypher_destroy( cypher );

  return RETURN_SUCCESS;
}
