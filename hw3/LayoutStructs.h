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

#ifndef HW3_LAYOUTSTRUCTS_H_
#define HW3_LAYOUTSTRUCTS_H_

#include <stdint.h>
#include <cstddef>

extern "C" {
  #include "./libhw1/CSE333.h"
  #include "./libhw2/MemIndex.h"
}
#include "./Utils.h"

// This header file defines the in-memory structs representing hw3 on-disk
// data structures.  They are "dumb data carriers" that require no additional
// transformations; they can be written and read directly from disk.  For
// that reason, the field ordering and each field's types must correspond
// exactly to the file format described in the HW3 spec.


// C/C++ will add padding to structures to place the individual
// fields on their natural byte alignment.  That can lead to
// trouble: the file must be read using the same structures as
// were used to write it.  Because that's so fragile, we
// turn off padding on the structures defined here:
#pragma pack(push, 1)

namespace hw3 {

//---------------------------------------------
// General types for encoded data on disk
//---------------------------------------------

// An offset within an index file.
typedef int32_t IndexFileOffset_t;


//---------------------------------------------
// Index file header
//---------------------------------------------

struct IndexFileHeader {
  uint32_t   magicNumber;    // the indicator of a well-formed header.
  uint32_t   checksum;       // the checksum for the entire file.
  int32_t    doctableBytes;  // number of bytes in the contained doctable.
  int32_t    indexBytes;     // number of bytes in the contained index.

  IndexFileHeader() { }  // this constructor yields uninitialized fields!

  IndexFileHeader(uint32_t magicNumberArg,
                  uint32_t checksumArg,
                  int32_t doctableBytesArg,
                  int32_t indexBytesArg)
    : magicNumber(magicNumberArg),
      checksum(checksumArg),
      doctableBytes(doctableBytesArg),
      indexBytes(indexBytesArg) {
  }

  void toDiskFormat() { magicNumber = htonl(magicNumber);
                        checksum = htonl(checksum);
                        doctableBytes = htonl(doctableBytes);
                        indexBytes = htonl(indexBytes);
                      }
  void toHostFormat() { magicNumber = ntohl(magicNumber);
                        checksum = ntohl(checksum);
                        doctableBytes = ntohl(doctableBytes);
                        indexBytes = ntohl(indexBytes);
                      }
};

// Offset of magic number within an IndexFileHeader struct, and also within
// the file itself.
#define MAGIC_NUMBER_OFFSET offsetof(IndexFileHeader, magicNumber)

// Offset of the doctable size field within an IndexFileHeader struct
#define DT_BYTES_OFFSET offsetof(IndexFileHeader, doctableBytes)


//---------------------------------------------
// Bucket lists
//  (Used by doctable, index, and docID table)
//---------------------------------------------

struct BucketListHeader {
  int32_t   numBuckets;  // number of buckets in the hash table

  BucketListHeader() { }  // this constructor doesn't initialize numBuckets!

  explicit BucketListHeader(int32_t numBucketsArg)
    : numBuckets(numBucketsArg) {
  }

  void toDiskFormat() { numBuckets = htonl(numBuckets); }
  void toHostFormat() { numBuckets = ntohl(numBuckets); }
};

struct BucketRecord {
  int32_t            chainNumElements;  // number of elements in this bucket's
                                        // chain.
  IndexFileOffset_t  position;          // byte offset from the start of the
                                        // index file, indicating where the
                                        //  bucket's chain begins.

  BucketRecord() { }  // this constructor doesn't initialize any fields!

  BucketRecord(int32_t numElts, IndexFileOffset_t pos)
    : chainNumElements(numElts), position(pos) {
  }

  void toDiskFormat() {
    chainNumElements = htonl(chainNumElements);
    position = htonl(position);
  }
  void toHostFormat() {
    chainNumElements = ntohl(chainNumElements);
    position = ntohl(position);
  }
};

struct ElementPositionRecord {
  IndexFileOffset_t  position;  // byte offset from the start of the index file,
                                // indicating where the element begins.

  ElementPositionRecord() { }
  explicit ElementPositionRecord(IndexFileOffset_t pos)
    : position(pos) {
  }

  void toDiskFormat() { position  = htonl(position); }
  void toHostFormat() { position = ntohl(position); }
};

//---------------------------------------------
// DocTable
//---------------------------------------------

struct DoctableElementHeader {
  DocID_t   docID;
  int16_t  filenameBytes;   // number of bytes needed to serialize the filename.
                            // note: 16-bit quantity!

  DoctableElementHeader() { }
  DoctableElementHeader(DocID_t id, int32_t numBytes)
    : docID(id), filenameBytes(numBytes) {
  }

  void toDiskFormat() {
    docID = htonll(docID);
    filenameBytes = htons(filenameBytes);
  }
  void toHostFormat() {
    docID = ntohll(docID);
    filenameBytes = ntohs(filenameBytes);
  }
};

//---------------------------------------------
// IndexTable
//
// Each element in the index is a WordPostings
// (see also HW2).
//---------------------------------------------

struct WordPostingsHeader {
  int16_t   wordBytes;      // number of bytes needed to serialize the word.
  int32_t   postingsBytes;  // number of bytes for the doctable.

  WordPostingsHeader() { }
  WordPostingsHeader(int16_t wordBytesArg, int32_t postingsBytesArg)
    : wordBytes(wordBytesArg), postingsBytes(postingsBytesArg) {
  }

  void toDiskFormat() {
    wordBytes = htons(wordBytes);
    postingsBytes = htonl(postingsBytes);
  }
  void toHostFormat() {
    wordBytes = ntohs(wordBytes);
    postingsBytes = ntohl(postingsBytes); }
};

//---------------------------------------------
// DocIDTable
//---------------------------------------------

struct DocIDElementHeader {
  DocID_t   docID;
  int32_t   numPositions;  // number of word positions stored in this element

  DocIDElementHeader() { }
  DocIDElementHeader(DocID_t id, int32_t numPos)
    : docID(id), numPositions(numPos) {
  }

  void toDiskFormat() {
    docID = htonll(docID);
    numPositions = htonl(numPositions);
  }
  void toHostFormat() {
    docID = ntohll(docID);
    numPositions = ntohl(numPositions);
  }
};

struct DocIDElementPosition {
  DocPositionOffset_t position;  // byte offset into the document referenced by
                                 // docID, indicating where the word occurs

  void toDiskFormat() { position = htonl(position); }
  void toHostFormat() { position = ntohl(position); }
};


}  // namespace hw3

#pragma pack(pop)

#endif  // HW3_LAYOUTSTRUCTS_H_
