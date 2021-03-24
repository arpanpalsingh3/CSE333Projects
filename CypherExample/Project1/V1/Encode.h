#ifndef SUBCYPHER_H
#define SUBCYPHER_H

/* Returns either a malloc()'ed string holding the encoded input text
   or NULL if some error. */
char* encode( const char *input_string, const char *cypher_vector );

#endif  // SUBCYPHER_H
