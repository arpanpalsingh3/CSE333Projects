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

#include "./WriteIndex.h"

#include <cstdio>    // for (FILE *).
#include <cstring>   // for strlen(), etc.

// We need to peek inside the implementation of a HashTable so
// that we can iterate through its buckets and their chain elements.
extern "C" {
  #include "libhw1/CSE333.h"
  #include "libhw1/HashTable_priv.h"
}
#include "./LayoutStructs.h"
#include "./Utils.h"

namespace hw3 {
//////////////////////////////////////////////////////////////////////////////
// Helper function declarations and constants

static constexpr int kFailedWrite = -1;

// Helper function to write the docid->filename mapping from the
// DocTable "dt" into file "f", starting at byte offset "offset".
// Returns the size of the written DocTable or a negative value on error.
static int WriteDocTable(FILE *f, DocTable *dt, IndexFileOffset_t offset);

// Helper function to write the MemIndex "mi" into file "f", starting
// at byte offset "offset."  Returns the size of the written MemIndex
// or a negative value on error.
static int WriteMemIndex(FILE *f, MemIndex *mi, IndexFileOffset_t offset);

// Helper function to write the index file's header into file "f".
// Returns the number of header bytes written on success, a negative
// value on failure.  Will atomically write the kMagicNumber as the
// very last thing; as a result, if we crash part way through writing
// an index file, it won't contain a valid kMagicNumber and the rest
// of HW3 will know to report an error.
static int WriteHeader(FILE *f,
                       int doctableBytes,
                       int memidxBytes);

// Function pointer used by WriteHashTable() to write a HashTable's
// HTKeyValue_t element into the index file at a specified byte offset.
//
// Returns:
//   - the number of bytes written, or a negative value on error.
typedef int (*WriteElementFn)(FILE *f,
                              IndexFileOffset_t offset,
                              HTKeyValue_t *kv);

// Writes a HashTable into the index file at a specified byte offset.
//
// Writes a header (BucketListHeader), a list of bucket records (BucketRecord),
// then the bucket contents themselves (using a content-specific instance of
// WriteElementFn).
//
// Since this function can write any hashtable, regardless of its contents,
// it is the main workhorse of the file.

// Arguments:
//   - f: the file to write into
//   - offset: the byte offset into "f", at which we should write the
//     hashtable
//   - ht: the hashtable to write
//   - fn: a function that serializes a single HTKeyValue_t.  Needs to be
//         specific to the hashtable's contents.
//
// Returns:
//   - the number of bytes written, or a negative value on error
static int WriteHashTable(FILE *f,
                          IndexFileOffset_t offset,
                          HashTable *ht,
                          WriteElementFn fn);

// Helper function used by WriteHashTable() to write a BucketRecord (ie, a
// "bucket_rec" within the hw3 diagrams).
//
// Arguments:
//   - f: the file to write into
//   - offset: the byte offset into "f", at which we should write the
//     BucketRecord
//   - numElts: the number of elements in the bucket.  Used to initialize
//     the BucketRecord's contents
//   - bucketOffset: the offset at which the bucket (not the BucketRecord) is
//     located in "f".
//
// Returns:
//   - the number of bytes written, or a negative value on error
static int WriteHTBucketRecord(FILE *f,
                               IndexFileOffset_t offset,
                               int32_t numElts,
                               IndexFileOffset_t bucketOffset);

// Helper function used by WriteHashTable() to write out a bucket.
//
// Remember that a bucket consists of a linked list of elements.  Thus, this
// function writes out a list of ElementPositionRecords, describing the
// location (as a byte offset) of each element, followed by a series of
// elements thesmelves (serialized using an element-specific WriteElementFn).
//
// Arguments:
//   - f: the file to write into
//   - offset: the byte offset into "f", at which we should write the bucket
//   - li: the bucket's contents
//   - fn: a function that serializes a single HTKeyValue_t -- stored as
//         the list's LLPayload_t -- within the linked list.
//
// Returns:
//   - the number of bytes written, or a negative value on error
static int WriteHTBucket(FILE *f,
                         IndexFileOffset_t offset,
                         LinkedList *li,
                         WriteElementFn fn);



//////////////////////////////////////////////////////////////////////////////
// "Writer" functions
//
// Functions that comply with the WriteElementFn signature, to be used when
// writing hashtable elements to disk.
static int WriteDocidToDocnameFn(FILE *f,
                                 IndexFileOffset_t offset,
                                 HTKeyValue_t *kv);


static int WriteWordToPostingsFn(FILE *f,
                                 IndexFileOffset_t offset,
                                 HTKeyValue_t *kv);

static int WriteDocidToPositionListFn(FILE *f,
                                      IndexFileOffset_t offset,
                                      HTKeyValue_t *kv);



//////////////////////////////////////////////////////////////////////////////
// WriteIndex

int WriteIndex(MemIndex *mi, DocTable *dt, const char *filename) {
  // Do some sanity checking on the arguments we were given.
  Verify333(mi != nullptr);
  Verify333(dt != nullptr);
  Verify333(filename != nullptr);

  // fopen() the file for writing; use mode "wb+" to indicate binary,
  // write mode, and to create/truncate the file.
  FILE *f = fopen(filename, "wb+");
  if (f == nullptr) {
    return kFailedWrite;
  }

  // Remember that the format of the index file is a header, followed by a
  // doctable, and then lastly a memindex.
  //
  // We write out the doctable and memindex first, since we need to know
  // their sizes before we can calculate the header.  So we'll skip over
  // the header for now.
  IndexFileOffset_t curPos = sizeof(IndexFileHeader);

  // Write the document table.
  int dtbytes = WriteDocTable(f, dt, curPos);
  if (dtbytes == kFailedWrite) {
    fclose(f);
    unlink(filename);
    return kFailedWrite;
  }
  curPos += dtbytes;

  // STEP 1.
  // Write the memindex.
  int membytes = WriteMemIndex(f, mi,curPos);
  if (membytes == kFailedWrite) {
    fclose(f);
    unlink(filename);
    return kFailedWrite;
  }
  curPos += membytes;


  // STEP 2.
  // Finally, backtrack to write the index header and write it.
  int headerbytes = WriteHeader(f, dtbytes,membytes);
  if (headerbytes == kFailedWrite) {
    fclose(f);
    unlink(filename);
    return kFailedWrite;
  }
  curPos += headerbytes;

  // Clean up and return the total amount written.
  fclose(f);
  return curPos;
}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static int WriteDocTable(FILE *f, DocTable *dt, IndexFileOffset_t offset) {
  // Break the DocTable abstraction in order to grab the docid->filename
  // hash table, then serialize it to disk.
  return WriteHashTable(f,
                        offset,
                        DT_GetDocidToDocnameTable(dt),
                        &WriteDocidToDocnameFn);
}

static int WriteMemIndex(FILE *f, MemIndex *mi, IndexFileOffset_t offset) {
  // Use WriteHashTable() to write the MemIndex into the file.  You'll
  // need to pass in the WriteWordToPostingsFn helper function as the
  // final argument.
  return WriteHashTable(f,
                        offset,
                        mi,
                        &WriteWordToPostingsFn);
}

static int WriteHeader(FILE *f,
                       int doctableBytes,
                       int memidxBytes) {
  // STEP 3.
  // We need to calculate the checksum over the doctable and index
  // table.  (Note that the checksum does not include the index file
  // header, just these two tables.)
  //
  // Use fseek() to seek to the right location, and use a CRC32 object
  // to do the CRC checksum calculation, feeding it characters that you
  // read from the index file using fread().
  // Seek to the start of the doctable.
  CRC32 crc;
  char feed;
  int totalBytes = doctableBytes + memidxBytes;

  // get location and return invalid const for error
  if (fseek(f, sizeof(IndexFileHeader), SEEK_SET) != 0) {
    return kFailedWrite;
  }
  // Read through each byte and fold it
  for (int i = 0; i < totalBytes; i++) {
    if(fread(reinterpret_cast<void*>(&feed), sizeof(feed), 1, f)  != 1) {
      return kFailedWrite;
    }
    crc.FoldByteIntoCRC(feed); // Fold byte
  }



  // Write the header fields.  Be sure to convert the fields to
  // network order before writing them!
  IndexFileHeader header(kMagicNumber, crc.GetFinalCRC(),
                         doctableBytes, memidxBytes);
  header.toDiskFormat();

  if (fseek(f, 0, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  if (fwrite(&header, sizeof(IndexFileHeader), 1, f) != 1) {
    return kFailedWrite;
  }

  // Use fsync to flush the header field to disk.
  Verify333(fsync(fileno(f)) == 0);

  // We're done!  Return the number of header bytes written.
  return sizeof(IndexFileHeader);
}

static int WriteHashTable(FILE *f,
                          IndexFileOffset_t offset,
                          HashTable *ht,
                          WriteElementFn fn) {
  // Write the HashTable's header, which consists simply of the number of
  // buckets.
  BucketListHeader header(ht->num_buckets);
  header.toDiskFormat();

  int totalWrite = 0; // Tracker for total write
  
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  if (fwrite(&header, sizeof(BucketListHeader), 1, f) != 1) {
    return kFailedWrite;
  } 
  // written header size
  totalWrite += sizeof(BucketListHeader);

  // The byte offset of the next BucketRecord we want to write.  Remember that
  // the bucket records are located after the bucket header.
  IndexFileOffset_t recordPos = offset + sizeof(BucketListHeader);

  // The byte offset of the next bucket we want to write.  Reember that
  // the buckets are placed after the bucket header and the entire list
  // of BucketRecords.
  IndexFileOffset_t bucketPos = offset + sizeof(BucketListHeader)
    + (ht->num_buckets) * sizeof(BucketRecord);

  // Iterate through the hashtable contents, first writing each bucket record
  // (ie, the BucketRecord) and then jumping forward in the file to write
  // the bucket contents itself.
  //
  // Be sure to handle the corner case where the bucket's chain is
  // empty.  For that case, you still have to write a record for the
  // bucket, but you won't write a bucket.

  int wRes; // Variable to capture bytes written 
  for (int i = 0; i < ht->num_buckets; i++) {
    // STEP 4.
    // Grab the bucket
    LinkedList* list = (ht->buckets)[i]; // go through the array of buckets 
    // Get the length of the bucket
    int len = LinkedList_NumElements(list);
    // Write the bucket record
    wRes = WriteHTBucketRecord(f,recordPos, len, bucketPos);
    if(wRes == kFailedWrite) {
      return kFailedWrite;
    }
    // Jump forward after writing so that the next record will go 4 bytes after
    recordPos += wRes;
    // Increment totalWrite 
    totalWrite += wRes;
    // Write the bucket 
    if(len != 0 ) { // dont write a bucket if its empty 
      wRes = WriteHTBucket(f,bucketPos,list,fn);
      if(wRes == kFailedWrite) {
        return kFailedWrite;
      }
      // Jump forward for next bucket
      bucketPos += wRes;
      // Increment total write
      totalWrite += wRes;
    }
  }

  // Calculate and return the total number of bytes written.
  return totalWrite;
}

static int WriteHTBucketRecord(FILE *f,
                               IndexFileOffset_t offset,
                               int32_t numElts,
                               IndexFileOffset_t bucketOffset) {
  // STEP 5.
  // Initialize a BucketRecord in network byte order.
  // Since toDiskFormat() can't be used in static functions, we use htonl
  BucketRecord record(numElts, bucketOffset);
  record.toDiskFormat();

  // fseek() to where we want to write this record.
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }

  // STEP 6.
  // Write the BucketRecord.
  int wRes = fwrite(&record,sizeof(BucketRecord),1,f);
  if(wRes != 1) {
    return kFailedWrite;
  }

  // Calculate and return how many bytes we wrote.
  return sizeof(BucketRecord);
}

static int WriteHTBucket(FILE *f,
                         IndexFileOffset_t offset,
                         LinkedList *li,
                         WriteElementFn fn) {
  int numElts = LinkedList_NumElements(li);
  if (numElts == 0) {
    return 0;
  }

  int totalWrite = 0; // Tacker for total write

  // The byte offset of the next ElementPositionRecord we want to write.
  IndexFileOffset_t recordPos = offset;

  // The byte offset of the next element we want to write.  Remember that
  // the elements are placed after the entire list of ElementPositionRecords.
  IndexFileOffset_t elementPos = offset
    + sizeof(ElementPositionRecord) * numElts;

  // Iterate through the list contents, first writing each entry's (ie, each
  // payload's) ElementPositionRecord and then jumping forward in the file
  // to write the element itself.
  //
  // Be sure to write in network order, and use the "fn" argument to write
  // the element (ie, the list payload) itself.

  int sRes,wRes,fnRes; // Seek, Write, Fn results
  LLIterator *it = LLIterator_Allocate(li); 
  Verify333(it != nullptr);
  for (int i = 0; i < numElts; i++) {
    HTKeyValue_t* kv;
    // STEP 7.
    // fseek() to the where the ElementPositionRecord should be written,
    // then fwrite() it in network order.
    
    // Seek to record position 
    sRes = fseek(f,recordPos, SEEK_SET);
    if(sRes != 0) {
      LLIterator_Free(it);
      return kFailedWrite;
    }
    // Write the EelementPositionRecord, which is just the elementPos
    // turn into elementpositionrecord, then toDiskFormat
    ElementPositionRecord elePos(elementPos);
    elePos.toDiskFormat();
    // Write the record
    wRes = fwrite(&elePos,sizeof(ElementPositionRecord),1,f);
    if(wRes != 1) {
      LLIterator_Free(it);
      return kFailedWrite;
    } 
    totalWrite += sizeof(ElementPositionRecord); // Increment totalWrite
    
    LLIterator_Get(it, (void **)(&kv));

    // STEP 8.
    // Write the element itself, using fn.
    fnRes = fn(f,elementPos,kv);
    if(fnRes == -1) {
      LLIterator_Free(it);
      return kFailedWrite;
    } 
    totalWrite += fnRes; // Increment totalWrite
    


    // Advance to the next element in the chain, updating our offsets.
    recordPos += sizeof(ElementPositionRecord);;
    elementPos += fnRes;  // increments by how many bytes we wrote for element
    LLIterator_Next(it);
  }
  LLIterator_Free(it);

  // Return the total amount of data written.
  return totalWrite; // total written bytes
}


//////////////////////////////////////////////////////////////////////////////
// "Writer" functions

// This write_element_fn is used to write a docid->docname mapping
// element, i.e., an element of the "doctable" table.
static int WriteDocidToDocnameFn(FILE *f,
                                 IndexFileOffset_t offset,
                                 HTKeyValue_t *kv) {
  // STEP 9.
  // determine the filename length
  char* filename = (char*)kv->value;
  int16_t filenamelen = strlen(filename);  
  // grab the legth of the file name string

  int totalWrite = 0; // tracker for all writes

  // fwrite() the docid from "kv".  Remember to convert to
  // disk format before writing.
  // kv->key is the DocId 
  DoctableElementHeader header(kv->key, filenamelen);
  // disk format = big endian 
  header.toDiskFormat(); 

  // fseek() to the provided offset and then write the header.
  if (fseek(f, offset, SEEK_SET) != 0) {
    return kFailedWrite;
  }
  int res = fwrite(&header, sizeof(DoctableElementHeader), 1, f);
  if (res != 1) {
    return kFailedWrite;
  } 
  totalWrite += sizeof(DoctableElementHeader);

  // STEP 10.
  // fwrite() the filename.  We don't write the null-terminator from the
  // string, just the characters, since we've already written a length
  // field for the string.
  int wRes;
  for(int i = 0; i < filenamelen; i++) {
    // Extract a byte
    char byte = filename[i];
    // Write said byte
    wRes = fwrite(&byte,sizeof(byte),1,f);
    if(wRes != 1) { 
      return kFailedWrite;
    } 
    totalWrite += sizeof(char); // Increment totalWrite
    
  }
  // STEP 11.
  // calculate and return the total amount written.
  return totalWrite ;  // Return total write
}

// This write_element_fn is used to write a DocID + position list
// element (i.e., an element of a nested docID table) into the file at
// offset 'offset'.
static int WriteDocidToPositionListFn(FILE *f,
                                      IndexFileOffset_t offset,
                                      HTKeyValue_t *kv) {
  // Extract the docID from the HTKeyValue_t.
  DocID_t docID = (DocID_t)kv->key; // 8 bytes

  int totalWrite = 0; // tracker for all writes

  // Extract the positions LinkedList from the HTKeyValue_t and
  // determine its size.
  LinkedList *positions = static_cast<LinkedList *>(kv->value);
  int numPositions = LinkedList_NumElements(positions); // 4 bytes 

  // STEP 12.
  // Write the header, in disk format.
  // You'll need to fseek() to the right location in the file.
  DocIDElementHeader header(docID,numPositions);
  header.toDiskFormat();

  int rRes;
  rRes = fseek(f,offset, SEEK_SET); // Seek to the right offset
  if(rRes != 0 ) {
    return kFailedWrite;
  }
  rRes = fwrite(&header,sizeof(DocIDElementHeader),1,f); // Write the header
  if(rRes != 1) {
    return kFailedWrite;
  } 
  totalWrite += sizeof(DocIDElementHeader); // increment total write

  // Loop through the positions list, writing each position out.
  DocIDElementPosition position;
  LLIterator *it = LLIterator_Allocate(positions);
  Verify333(it != nullptr);
  for (int i = 0; i < numPositions; i++) {
    // STEP 13.
    // Get the next position from the list.
    LLIterator_Get(it, reinterpret_cast<void **>(&position) );


    // STEP 14.
    // Truncate to 32 bits, then convert it to network order and write it out.
    position.position = static_cast<int32_t>(position.position);
    position.toDiskFormat();

    int wRes;
    // Write the position 
    wRes = fwrite(&position,sizeof(DocIDElementPosition), 1, f);
    if(wRes != 1) {
      LLIterator_Free(it);
      return kFailedWrite;
    } 
    totalWrite += sizeof(DocIDElementPosition); // increment total write

    // Advance to the next position.
    LLIterator_Next(it);
  }
  LLIterator_Free(it);

  // STEP 15.
  // Calculate and return the total amount of data written.
  return totalWrite;  // return total write
}

// This write_element_fn is used to write a WordPostings
// element into the file at position 'offset'.
static int WriteWordToPostingsFn(FILE *f,
                                 IndexFileOffset_t offset,
                                 HTKeyValue_t *kv) {
  // Extract the WordPostings from the HTKeyValue_t.
  WordPostings *wp = static_cast<WordPostings *>(kv->value);
  Verify333(wp != nullptr);

  int totalWrite = 0; // tracker for all writes
  // STEP 16.
  // Prepare the wordlen field.
  char* word = wp->word; // Extract the word into a char*
  int16_t wordBytes = strlen(wp->word);  // length of word 

  // Write the nested DocID->positions hashtable (i.e., the "docID
  // table" element in the diagrams).  Use WriteHashTable() to do it,
  // passing it the wp->postings table and using the
  // WriteDocidToPositionListFn helper function as the final parameter.
  int htBytes = WriteHashTable(f,
                               offset + sizeof(WordPostingsHeader) + wordBytes,
                               wp->postings,
                               &WriteDocidToPositionListFn);

  if(htBytes == -1) { // make sure the table was written
    return kFailedWrite;
  }
  totalWrite += htBytes; // Increment totalWrite
  // STEP 17.
  // Write the header, in network order, in the right place in the file.
  WordPostingsHeader header(wordBytes, htBytes);
  header.toDiskFormat();
  int sRes, wRes; // Seek, write result
  sRes = fseek(f, offset,SEEK_SET);
  if(sRes != 0) {
    return kFailedWrite;
  }
  // write the header
  wRes = fwrite(&header,sizeof(WordPostingsHeader),1,f);
  if(wRes != 1) {
    return kFailedWrite;
  } 
  totalWrite += sizeof(WordPostingsHeader); // increment total write

  // STEP 18.
  // Write the word itself, excluding the null terminator, in the right
  // place in the file.
  for(int i = 0; i < wordBytes; i++) {
    // extract individual char from word
    char chr = word[i];
    // write each char 
    wRes = fwrite(&chr, sizeof(char),1,f);
    if(wRes != 1) {
      return kFailedWrite;
    } 
    
    totalWrite += sizeof(char); // increment total write
  }


  // STEP 19.
  // Calculate and return the total amount of data written.
  return totalWrite; // return totalWrite
}
}  // namespace hw3
