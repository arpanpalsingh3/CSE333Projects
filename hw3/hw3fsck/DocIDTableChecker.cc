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

#include "./DocIDTableChecker.h"

#include <assert.h>     // for assert()
#include <arpa/inet.h>  // for ntohl(), etc.
#include <stdint.h>     // for uint32_t, etc.
#include <string>

extern "C" {
  #include "libhw2/DocTable.h"
}
#include "../Utils.h"
#include "./CheckerUtils.h"

using std::string;

namespace hw3 {

// The constructor for DocIDTableChecker calls the constructor
// of HashTableChecker(), its superclass. The superclass takes
// care of taking ownership of f and using it to extract and
// cache the number of buckets within the table.
DocIDTableChecker::DocIDTableChecker(FILE *f, int32_t offset, int32_t len,
                                     const string &context)
  : HashTableChecker(f, offset, len), context_(context) { }

bool DocIDTableChecker::CheckElement(int32_t elementOffset,
                                     int32_t bucketNumber) {
  bool result = true;
  // seek to the start of the element
  assert(fseek(file_, elementOffset, SEEK_SET) == 0);

  // read the docID
  DocID_t docID;
  assert(fread(&docID, sizeof(DocID_t), 1, file_) == 1);
  docID = ntohll(docID);

  // make sure the docID is in the right bucket
  result &= CheckEQ64(bucketNumber, docID % numBuckets_,
            "[DocID table] DocID % num_buckets == bucket_number");

  // read in the number of positions
  int32_t numPos;
  assert(fread(&numPos, sizeof(int32_t), 1, file_) == 1);
  numPos = ntohl(numPos);
  result &= CheckLT32(numPos, 1000000, "[DocID table] num_positions");

  // loop through and check the positions
  int32_t prevPos;
  for (int32_t i = 0; i < numPos; i++) {
    int32_t curPos;
    assert(fread(&curPos, sizeof(int32_t), 1, file_) == 1);
    curPos = ntohl(curPos);
    if (i > 0) {
      result &= CheckLT32(prevPos,
                curPos,
                "word position[i] < word position[i+1]");
    }
    prevPos = curPos;
  }
  return result;
}
}  // namespace hw3
