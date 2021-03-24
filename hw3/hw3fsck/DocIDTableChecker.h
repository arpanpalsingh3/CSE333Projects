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

#ifndef HW3_HW3FSCK_DOCIDTABLECHECKER_H_
#define HW3_HW3FSCK_DOCIDTABLECHECKER_H_

#include <stdint.h>  // for uint32_t, etc.
#include <cstdio>    // for (FILE *)
#include <string>

#include "../Utils.h"
#include "./HashTableChecker.h"

namespace hw3 {

// A DocIDTableChecker (a subclass of HashTableChecker) is used to
// read one of the many the embedded docid-->positions "docIDtable"
// tables within the index file.
class DocIDTableChecker : public HashTableChecker {
 public:
  // Construct a new DocIDTableChecker at a specific offset with an
  // index file.  Arguments:
  //
  // - f: an open (FILE *) for the underlying index file.  The
  //   constructed object takes ownership of the (FILE *) and will
  //   fclose() it  on destruction.
  //
  // - offset: the "docIDtable"'s byte offset within the file.
  //
  // - context: a context string to pass in to the Check*() functions.
  DocIDTableChecker(FILE *f, int32_t offset, int32_t len,
                    const std::string &context);

  // Check a DocIDTableElement.
  // Returns true if the element seems correct, false otherwise.
  virtual bool CheckElement(int32_t elementOffset,
                            int32_t bucketNumber);

 private:
  std::string context_;

  DISALLOW_COPY_AND_ASSIGN(DocIDTableChecker);
};

}  // namespace hw3

#endif  // HW3_HW3FSCK_DOCIDTABLECHECKER_H_
