#ifndef READCYPHER_H
#define READCYPHER_H

/* Returns a char[256] encoding the cypher given in the file, or NULL on error.
   Client must free() the returned array. */
char* CreateCypher(const char* filename);

#endif  // READCYPHER_H
