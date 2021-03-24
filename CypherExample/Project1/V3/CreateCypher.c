#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#include "CreateCypher.h"

/* Maybe this should go in a .h? */
typedef unsigned char BYTE;

/* Create cypher map from description in file.
   File has pairs of non-space characters indicating a
   encoding of the first of the pair as the second. */
char* CreateCypher(const char* filename)
{
  char* cypher_table = NULL;
  FILE* in_file;
  
  if ( filename != NULL && (in_file = fopen(filename, "r")) != NULL )
    {
      /* Allocate and initialize cypher table. */
      cypher_table = (char*)malloc(256);
      for ( int i=0; i<256; i++ )
	cypher_table[i] = i;

      /* Read each successive non-blank string from the file and process. */
      char *nextPair;
      while ( fscanf(in_file, "%ms", &nextPair)==1 )
	{
	  if ( strlen(nextPair) != 2 )
	    {
	      fprintf(stderr, "Error: bad cypher file entry: '%s'\n", nextPair);
	      free(nextPair);
	      free(cypher_table);
	      cypher_table = NULL;
	      break;
	    }
	  cypher_table[(BYTE)nextPair[0]] = nextPair[1];
	  free(nextPair);
	}
    }

  return cypher_table;
}
