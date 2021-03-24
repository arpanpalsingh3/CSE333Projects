#ifndef READCYPHER_H
#define READCYPHER_H

/* Here we use C to implement an object oriented design.
   Of course, C doesn't have objects, so we have to use
   some conventions.  First, an "object" is a pointer to
   a struct. */
typedef struct Cypher_t * Cypher;

/* Here's the constructor.  It malloc()s space. */
Cypher Cypher_new(const char* filename);

/* The client must call this method when done with the object.
   It frees space. */
void Cypher_destroy(Cypher cypher);

/* The only method we implement uses the cypher to encode a string.
   The client is responsible for free()ing the space returned.
   (This was formerly the encode() function.) */
char*  Cypher_encode( Cypher cypher, const char *input_string );

#endif  // READCYPHER_H
