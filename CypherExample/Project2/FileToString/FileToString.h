#ifndef FILETOSTRING_H
#define FILETOSTRING_H

// Returns the contents of the named file or NULL on error.
// Client must free() returned string.

char* FileToString(const char* filename);

#endif  // FILETOSTRING_H
