#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

#include "Encode.h"

void usage(const char* exeName) {
  fprintf(stderr, "Usage: %s\n", exeName);
}

int main(int argc, char* argv[]) {
  char *input_string;    // the input we'll give the encoder
  char cypher[256];      // holds the translation table (from input to output character)
  char *encoded_text;    // will point to encoded string

  /* Check usage. */
  if ( argc != 1 )
    {
      usage(argv[0]);
      return EXIT_FAILURE;
    }
  
  /* Initialize input string. */
  input_string = "I wish I had taken CSE 333 last quarter.";

  /* Our cypher just converts 'a' to 'Z' and leaves everything else unchanged. */
  for (int i=0; i<256; i++)
    cypher[i] = i;
  cypher['a'] = 'Z';

  /* Encode the text. */
  encoded_text = encode( input_string, cypher );

  /* Print output. */
  printf("%20s '%s'\n", "Encoding of", input_string);
  if ( encoded_text )
    printf("%20s '%s'\n", "is", encoded_text );
  else
    printf("encountered an error\n");

  /* To highlight the design "feature" that the subroutine malloc's
     and so the client must free, we do the free here.
     It might seem useless -- the program is about to terminate and
     the virtual address space will be destroyed, so why bother?
     (a) It's good programming practice to free what you allocate,
         and if this mainline changes it might not terminate right
	 away here, and
     (b) more importantly, speaking practically, you want to check
         for memory leaks using a tool (valgrind), and that is simplest
	 if the tool reports no memory leaks rather than it reporting leaks
	 that you then have to look at the source and say, "Well, okay, that
	 leak is benign."
     NOTE: I'm relying on free accepting NULL as the argument.  
     See 'man 3 free' to decide whether that assumption is okay or not. */
  free( encoded_text );

  /* This mainline never fails... */
  return EXIT_SUCCESS;
}
