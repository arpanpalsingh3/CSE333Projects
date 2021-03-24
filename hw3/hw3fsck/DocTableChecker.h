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

#ifndef HW3_HW3FSCK_DOCTABLECHECKER_H_
#define HW3_HW3FSCK_DOCTABLECHECKER_H_

#include <stdint.h>  // for uint32_t, etc.
#include <cstdio>    // for (FILE *)

#include "../Utils.h"
#include "./HashTableChecker.h"

namespace hw3 {

// A DocTableChecker (a subclass of HashTableChecker) is used to
// read the single docid-->docname "doctable" within the index file.
class DocTableChecker : public HashTableChecker {
 public:
  // Construct a new DocTableChecker at a specified offset within
  // an index file.  Arguments:
  //
  // - f: an open (FILE *) for the underlying index file.  The
  //   constructed  object takes ownership of the (FILE *) and will
  //   fclose() it on destruction.
  //
  // - offset: the "doctable"'s byte offset within the file.
  DocTableChecker(FILE *f, int32_t offset, int32_t len);

  // Check a DocTableElement.
  // Returns true if the element seems correct, false otherwise.
  virtual bool CheckElement(int32_t elementOffset,
                            int32_t bucketNumber);

 private:
  DISALLOW_COPY_AND_ASSIGN(DocTableChecker);
};

}  // namespace hw3

#endif  // HW3_HW3FSCK_DOCTABLECHECKER_H_
