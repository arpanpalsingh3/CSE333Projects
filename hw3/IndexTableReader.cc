/*
 * Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./IndexTableReader.h"

#include <stdint.h>     // for uint32_t, etc.
#include <string>       // for std::string.
#include <sstream>      // for std::stringstream.

extern "C" {
  #include "libhw1/HashTable.h"  // for libhw1/hashtable.h's FNVHash64().
  #include "libhw1/CSE333.h"
}
#include "./Utils.h"   // for FileDup().


using std::string;
using std::stringstream;

namespace hw3 {

// The constructor for IndexTableReader calls the constructor of
// HashTableReader(), its superclass. The superclass takes care of
// taking ownership of f and using it to extract and cache the number
// of buckets within the table.
IndexTableReader::IndexTableReader(FILE *f, IndexFileOffset_t offset)
  : HashTableReader(f, offset) { }

DocIDTableReader* IndexTableReader::LookupWord(const string &word) {
  // Calculate the FNVHash64 of the word.  Use word.c_str() to get a
  // C-style (char *) to pass to FNVHash64, and word.lengt() to figure
  // out how many characters are in the string.
  HTKey_t wordhash = FNVHash64((unsigned char *) word.c_str(),
                                word.length());

  // Get back the list of "element" offsets for this word hash.
  auto elements = LookupElementPositions(wordhash);

  // If the list is empty, we're done; return nullptr.
  if (elements.empty()) {
    return nullptr;
  }

  // Iterate through the elements.
  for (auto it = elements.begin(); it != elements.end(); it++) {
    IndexFileOffset_t offset = *it;

    // STEP 1.
    // Slurp the header information out of the "element" field;
    // specifically, extract the "word length" field and the "docID
    // table length" fields, converting from network to host order.
    WordPostingsHeader header;
    // Seek to offset
    Verify333(fseek(file_,offset,SEEK_SET) == 0);
    // Read into header, convert to host
    fread(&header,sizeof(WordPostingsHeader),1,file_);
    header.toHostFormat();


    // If the "word length" field doesn't match the length of the word
    // we're looking up, use continue to skip to the next element.
    if (header.wordBytes != (signed) word.length()) {
      continue;
    }

    // We might have a match for the word. Read the word itself, using
    // the "<<" operator to feed a std::stringstream characters read
    // using fread().
    stringstream ss;
    for (int i = 0; i < header.wordBytes; i++) {
      // STEP 2.
      char nextChar;
      // read 1 char at a time in the for loop
      fread(&nextChar,sizeof(char),1,file_);
      // add it to the stringstream
      ss << nextChar;
    }

    // Use ss.str() to extract a std::string from the stringstream,
    // and use the std::string's "compare()" method to see if the word
    // we read from the "element" matches our "word" parameter.
    if (word.compare(ss.str()) == 0) {
      // If it matches, use "new" to heap-allocate and manufacture a
      // DocIDTableReader.  Be sure to use FileDup() to pass a
      // duplicated (FILE *) as the first argument to the
      // DocIDTableReader constructor, since we want the manufactured
      // DocIDTableReader to have its own (FILE *) handle
      //
      // return the new'd (DocIDTableReader *) to the caller.
      IndexFileOffset_t docIDTableOffset = offset + sizeof(WordPostingsHeader)
                                                  + header.wordBytes;
      DocIDTableReader *ditr = new DocIDTableReader(FileDup(file_),
                                                    docIDTableOffset);
      return ditr;
    }
  }
  return nullptr;
}

}  // namespace hw3
