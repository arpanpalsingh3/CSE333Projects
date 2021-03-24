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

#include <stdint.h>     // for uint32_t, etc.
#include <sstream>      // for std::stringstream

#include "./DocTableReader.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

using std::string;
using std::stringstream;

namespace hw3 {

// The constructor for DocTableReader calls the constructor
// of HashTableReader(), its superclass. The superclass takes
// care of taking ownership of f and using it to extract and
// cache the number of buckets within the table.
DocTableReader::DocTableReader(FILE *f, IndexFileOffset_t offset)
  : HashTableReader(f, offset) { }

bool DocTableReader::LookupDocID(const DocID_t &docid,
                                 string *retval) {
  // Use the superclass's "LookupElementPositions" function to
  // walk through the doctable and get back a list of offsets
  // to elements in the bucket for this docID.
  auto elements = LookupElementPositions(docid);

  // If the list is empty, we're done.
  if (elements.empty())
    return false;

  // Iterate through the elements, looking for our docID.
  for (auto it = elements.begin(); it != elements.end(); it++) {
    IndexFileOffset_t curr = *it;

    // STEP 1.
    // Slurp the next docid out of the element.
    DoctableElementHeader currHeader;
    Verify333(fseek(file_,curr,SEEK_SET) == 0); // Seek to desired offset
    fread(&currHeader, sizeof(DoctableElementHeader), 1, file_); // Read into curHeader
    currHeader.toHostFormat(); // To host format 

    // Is it a match?
    if (currHeader.docID == docid) {
      // Yes!  Extract the filename, using a stringstream and its "<<"
      // operator, fread()'ing a character at a time.
      stringstream ss;
      for (int i = 0; i < currHeader.filenameBytes; i++) {
        uint8_t nextc;

        Verify333(fread(&nextc, sizeof(char), 1, file_) == 1);
        ss << nextc;
      }

      // STEP 2.
      // Using the str() method of ss to extract a std::string object,
      // and return it through the output parameter retval.  Return
      // true.
      
      *retval = ss.str();

      return true;
    }
  }

  // We failed to find a matching docID, so return false.
  return false;
}

}  // namespace hw3
