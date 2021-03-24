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

#include "./DocTableChecker.h"

#include <assert.h>     // for assert()
#include <arpa/inet.h>  // for ntohl(), etc.
#include <stdint.h>     // for uint32_t, etc.
#include <string>
#include <cctype>       // for isascii

extern "C" {
  #include "../libhw2/DocTable.h"
}
#include "../Utils.h"  // for ntohll()
#include "./CheckerUtils.h"

using std::string;

namespace hw3 {

// The constructor for DocTableChecker its superclass constructor,
// which takes ownership of f, and uses it to extract and cache
// the number of buckets within the table.
DocTableChecker::DocTableChecker(FILE *f, int32_t offset, int32_t len)
  : HashTableChecker(f, offset, len) { }

bool DocTableChecker::CheckElement(int32_t elementOffset,
                                   int32_t bucketNumber) {
  bool result = true;
  // Read in the docID
  uint64_t docID;
  assert(fseek(file_, elementOffset, SEEK_SET) == 0);
  assert(fread(&docID, sizeof(DocID_t), 1, file_) == 1);
  docID = ntohll(docID);

  // Make sure the docID makes sense for this bucket.
  result &= CheckEQ32(bucketNumber, docID % numBuckets_,
            "[DocTable] DocID % num_buckets == bucket_number");

  // Read in the filename length.
  int16_t fnlen;
  assert(fread(&fnlen, sizeof(int16_t), 1, file_) == 1);
  fnlen = ntohs(fnlen);
  CheckLT16(fnlen, 1024, "[DocTable] filename length");

  // Read in the filename.
  char filename[fnlen+1];
  filename[fnlen] = '\0';
  assert(fread(&filename[0], fnlen, 1, file_) == 1);

  // Make sure the filename is all ASCII.
  for (int i = 0; i < fnlen; i++) {
    result &= CheckEQ16(1, (isascii(filename[i]) ? 1 : 0),
                        ToString("[DocTable] isascii(filename)[%d]", i));
  }
  return result;
}

}  // namespace hw3
