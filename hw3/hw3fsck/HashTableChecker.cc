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

#include "./HashTableChecker.h"

#include <arpa/inet.h>        // for ntohl().
#include <assert.h>           // for assert().
#include <iostream>

#include "../Utils.h"
#include "./CheckerUtils.h"

namespace hw3 {

HashTableChecker::HashTableChecker(FILE *f, int32_t offset, int32_t len)
  : file_(f), offset_(offset), len_(len) {
}

HashTableChecker::~HashTableChecker() {
  // fclose our (FILE *).
  fclose(file_);
  file_ = NULL;
}

bool HashTableChecker::Check(int32_t offset, int32_t len) {
  // fread() the number of buckets in this hashtable from its
  // "num_buckets" field.
  bool result = true;

  assert(fseek(file_, offset_, SEEK_SET) == 0);
  assert(fread(&numBuckets_, sizeof(numBuckets_), 1, file_) == 1);

  // Convert the num_buckets_ from network (on-disk) representation to
  // host (in-memory) byte ordering using ntohl().
  numBuckets_ = ntohl(numBuckets_);

  // Check that the number of buckets is reasonable.
  result &= CheckLT32(numBuckets_ * kBucketRecordSize,
                      len, "num_buckets < len(table)");

  // Loop through all of the bucket records, checking each bucket.
  bool foundFirstNonEmpty = false;
  int32_t prevChainLen, prevBucket;

  for (int32_t i = 0; i < numBuckets_; i++) {
    int32_t curChainLen, curBucket;

    // read the bucket records
    assert(fseek(file_, offset_ + kBucketLengthSize + (kBucketRecordSize*i),
                 SEEK_SET) == 0);
    assert(fread(&curChainLen, sizeof(int32_t), 1, file_) == 1);
    assert(fread(&curBucket, sizeof(int32_t), 1, file_) == 1);
    curChainLen = ntohl(curChainLen);
    curBucket = ntohl(curBucket);

    // check against prev bucket offset for sanity
    if (foundFirstNonEmpty && (curChainLen > 0)) {
      result &= CheckLT32(prevBucket, curBucket,
                  ToString("bucket_rec[%d].position < bucket_rec[%d].position",
                  i, i+1));
      prevChainLen = curChainLen;
      prevBucket = curBucket;
    }

    // if this is the first non-empty bucket, read its offset from the
    // bucket record to make sure it lines up with the number of
    // records we have.
    if ((curChainLen > 0) && (!foundFirstNonEmpty)) {
      foundFirstNonEmpty = true;

      assert(fseek(file_, offset_ + kBucketLengthSize, SEEK_SET) == 0);
      assert(fread(&prevChainLen, sizeof(int32_t), 1, file_) == 1);
      prevChainLen = ntohl(prevChainLen);
      assert(fread(&prevBucket, sizeof(int32_t), 1, file_) == 1);
      prevBucket = ntohl(prevBucket);

      result &= CheckEQ32(offset_ + kBucketLengthSize +
                kBucketRecordSize*numBuckets_, prevBucket,
                "position of the first non-empty bucket (expected to be "
                "(table start) + (4) + (8*num_buckets))");
    }

    // Seek to the bucket, make sure there are three element position
    // records that make sense.
    int32_t tableEnd = offset + len;
    CheckLT32(curBucket, tableEnd + 1,
              ToString("bucket_rec[%d].position < table_end+1", i));
    for (int j = 0; j < curChainLen; j++) {
      // read the element position
      int32_t elementPos;
      assert(fseek(file_, curBucket + j*kElementPositionSize, SEEK_SET) == 0);
      assert(fread(&elementPos, sizeof(int32_t), 1, file_) == 1);
      elementPos = ntohl(elementPos);
      result &= CheckLT32(elementPos, tableEnd + 1,
                ToString("bucket[%d].element[%d].position < table_end+1",
                i, j));

      // read the element itself
      result &= CheckElement(elementPos, i);
    }
  }  // end loop over all buckets
  return result;
}

}  // namespace hw3
