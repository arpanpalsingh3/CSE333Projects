#ifndef CYPHER_H
#define CYPHER_H

/* Here we use C to implement an object oriented design.
   Of course, C doesn't have objects, so we have to use
   some conventions.  First, an "object" is a pointer to
   a struct. */
typedef struct Cypher_t * Cypher;

/* Here's the constructor.  It malloc()s space. */
Cypher Cypher_new(const char* filename);

/* The client must call this method when done with the object.
   It frees space (plus anything else the object needs to do,
   which for Cypher is nothing). */
void Cypher_destroy(Cypher cypher);

/* The only method we implement uses the cypher read from
   the file to encode a string.
   (This was formerly the encode() function.)                  */
char* Cypher_encode( Cypher cypher, const char *input_string );

#endif  // CYPHER_H
