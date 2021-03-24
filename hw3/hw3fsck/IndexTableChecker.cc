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

#include "./IndexTableChecker.h"

#include <assert.h>     // for assert().
#include <arpa/inet.h>  // for ntohl(), ntohs().
#include <stdint.h>     // for uint32_t, etc.
#include <cctype>       // for isascii, etc.
#include <sstream>      // for std::stringstream.
#include <string>       // for std::string.

#include "../Utils.h"
#include "./CheckerUtils.h"
#include "./DocIDTableChecker.h"

using std::string;

namespace hw3 {

// The constructor for IndexTableChecker calls the constructor of
// HashTableChecker(), its superclass. The superclass takes care of
// taking ownership of f and using it to extract and cache the number
// of buckets within the table.
IndexTableChecker::IndexTableChecker(FILE *f, int32_t offset, int32_t len)
  : HashTableChecker(f, offset, len) { }

bool IndexTableChecker::CheckElement(int32_t elementOffset,
                                     int32_t bucketNumber) {
  bool result = true;

  // Read in the word length.
  int16_t wordLen;
  assert(fseek(file_, elementOffset, SEEK_SET) == 0);
  assert(fread(&wordLen, sizeof(int16_t), 1, file_) == 1);
  wordLen = ntohs(wordLen);
  result &= CheckLT16(wordLen, 8192,
            ToString("[IndexTable] unreasonably long word in bucket[%d]",
                     bucketNumber));

  // Read in the docID table length.
  int32_t docIDTableLen;
  assert(fread(&docIDTableLen, sizeof(int32_t), 1, file_) == 1);
  docIDTableLen = ntohl(docIDTableLen);
  result &= CheckLT32(elementOffset + kWordLenSize + kDocIDTableLengthSize +
            wordLen, offset_ + len_,
            ToString("[IndexTable] element_end < indextable_end in bucket[%d]",
                     bucketNumber));

  // Read in the word.
  char word[wordLen+1];
  word[wordLen] = '\0';
  assert(fread(&word[0], wordLen, 1, file_) == 1);

  // Make sure the word is all ascii and lower case.
  for (int i = 0; i < wordLen; i++) {
    result &= CheckEQ16(1, (isascii(word[i]) ? 1 : 0),
              ToString("[IndexTable] isascii(word)[%d] in bucket[%d]",
                       i, bucketNumber));
    if (!isalpha(word[i])) {
      continue;
    }
    result &= CheckEQ16(1, (islower(word[i]) ? 1 : 0),
              ToString("[IndexTable] islower(word)[%d] in bucket[%d]",
                     i, bucketNumber));
  }

  DocIDTableChecker ditc(FileDup(file_),
                         elementOffset + kWordLenSize + kDocIDTableLengthSize
                         + wordLen,
                         docIDTableLen,
                         ToString("DocIDtable for word '%s',"
                                  " embedded in bucket[%d]",
                                  word, bucketNumber));
  result &= ditc.Check(elementOffset + kWordLenSize + kDocIDTableLengthSize +
             wordLen, docIDTableLen);
  return result;
}

}  // namespace hw3
