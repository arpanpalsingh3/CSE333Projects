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

#ifndef HW3_HW3FSCK_HASHTABLECHECKER_H_
#define HW3_HW3FSCK_HASHTABLECHECKER_H_

#include <stdint.h>  // for uint32_t, etc.
#include <cstdio>    // for (FILE *).
#include <list>      // for std::list.

#include "../Utils.h"

namespace hw3 {

// A HashTableChecker is the base class for the different kinds
// of hash table readers.  Its subclasses are the DocTableChecker, the
// IndexTableChecker, and the DocPositionsReader.
class HashTableChecker {
 public:
  // Construct a HashTableChecker reader.  Arguments:
  //
  // - f: an open (FILE *) for the underlying index file.  The
  //   constructed object takes ownership of the (FILE *) and will
  //   fclose() it on destruction.
  //
  // - offset: the hash table's byte offset within the file.
  HashTableChecker(FILE *f, int32_t offset, int32_t len);
  virtual ~HashTableChecker();

  // This function checks the hashtable for consistency.
  // Returns true if the hashtable seems correct, false otherwise.
  virtual bool Check(int32_t offset, int32_t len);

  // This function is invoked by the checker when it is time for
  // it to evaluate an element within the hashtable.  Derived classes
  // need to override this function to provide table-type-specific
  // element checking. bucket_number is provided to verify that the
  // element's hash value lines up with the bucket correctly.
  // Returns true if the element seems correct, false otherwise.
  virtual bool CheckElement(int32_t elementOffset,
                            int32_t bucketNumber) = 0;

 protected:
  // The open (FILE *) stream associated with this hash table.
  FILE *file_;

  // The byte offset within the file that this hash table starts at.
  int32_t offset_;

  // The length of the table, in bytes.
  int32_t len_;

  // A cached copy of the total number of buckets in this hash table.
  int32_t numBuckets_;

 private:
  DISALLOW_COPY_AND_ASSIGN(HashTableChecker);
};

}  // namespace hw3

#endif  // HW3_HW3FSCK_HASHTABLECHECKER_H_
