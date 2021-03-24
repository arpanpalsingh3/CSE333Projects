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

#include "./FileIndexChecker.h"

#include <assert.h>     // for assert
#include <sys/types.h>  // for stat()
#include <sys/stat.h>   // for stat()
#include <unistd.h>     // for stat()
#include <iostream>     // for cout, endl
#include "../Utils.h"   // for class CRC32.
#include "./CheckerUtils.h"
#include "./DocTableChecker.h"
#include "./IndexTableChecker.h"

using std::cout;
using std::endl;
using std::hex;
using std::string;

namespace hw3 {

FileIndexChecker::FileIndexChecker(const string &filename) {
  // Stash a copy of the index file's name.
  filename_ = filename;

  // Open a (FILE *) associated with filename.  Crash on error.
  file_ = fopen(filename_.c_str(), "rb");
  assert(file_ != nullptr);

  // Make the (FILE *) be unbuffered.  ("man setbuf")
  setbuf(file_, nullptr);

  // read in the doctableSize_ and indexSize_ fields.
  int offset = kMagicNumberOffset + sizeof(uint32_t) + sizeof(uint32_t);
  fseek(file_, offset, SEEK_SET);

  // Read the doctable size field from the index file.
  assert(fread(&doctableSize_, sizeof(doctableSize_), 1, file_) == 1);
  doctableSize_ = ntohl(doctableSize_);

  // Read the index size field from the index file.
  assert(fread(&indexSize_, sizeof(indexSize_), 1, file_) == 1);
  indexSize_ = ntohl(indexSize_);
}

bool FileIndexChecker::CheckFile() {
  // Is the file we are checking good?
  bool fileGood = true;

  cout << "hw3fsck'ing " << filename_ << endl;

  fileGood &= this->CheckHeader();
  fileGood &= this->CheckDocTable();
  fileGood &= this->CheckIndexTable();

  cout << "  done" << endl;

  return fileGood;
}

bool FileIndexChecker::CheckHeader() {
  bool headerGood = true;

  cout << "  checking the header..." << endl;

  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags ff = cout.flags();
  cout << hex;

  // Read the magic number header field from the index file, verify
  // that it is correct.  Warn if not.
  cout << "    checking the magic number..." << endl;
  uint32_t magicNumber;
  assert(fseek(file_, kMagicNumberOffset, SEEK_SET) == 0);
  assert(fread(&magicNumber, sizeof(magicNumber), 1, file_) == 1);
  magicNumber = ntohl(magicNumber);
  headerGood &= CheckEQ32(kMagicNumber, magicNumber, "magic number");

  // Read the checksum header field from the index file.
  uint32_t checksum;
  assert(fread(&checksum, sizeof(checksum), 1, file_) == 1);
  checksum = ntohl(checksum);

  // Make sure the index file's length lines up with the header fields.
  cout << "    checking file size against table offsets..." << endl;
  struct stat fStat;
  assert(stat(filename_.c_str(), &fStat) == 0);
  headerGood &= CheckEQ32(fStat.st_size - kFileIndexHeaderSize,
                        doctableSize_ + indexSize_,
                        "doctable_size + index_size");
  if ((ntohl(doctableSize_) + ntohl(indexSize_)) ==
      fStat.st_size - kFileIndexHeaderSize) {
    cout << "Note: you likely forgot an endianness conversion, ";
    cout << "since ntohl(doctable_size_) + ntohl(index_size_) == ";
    cout << "filesize - 16" << endl;
  }

  // Re-calculate the checksum, make sure it matches that in the header.
  cout << "    recalculating the checksum..." << endl;
  fseek(file_, kFileIndexHeaderSize, SEEK_SET);
  CRC32 crc;
  constexpr int kBufSize = 512;
  uint8_t buf[kBufSize];
  int32_t bytesLeft =
    fStat.st_size - kFileIndexHeaderSize;
  while (bytesLeft > 0) {
    int bytesRead = fread(&buf[0], sizeof(uint8_t), kBufSize, file_);
    assert(bytesRead > 0);
    for (int i = 0; i < bytesRead; i++) {
      crc.FoldByteIntoCRC(buf[i]);
    }
    bytesLeft -= bytesRead;
  }
  headerGood &= CheckEQ32(crc.GetFinalCRC(), checksum, "checksum");

  // set the flags of cout to what they were before we changed them here.
  cout.flags(ff);
  return headerGood;
}

bool FileIndexChecker::CheckDocTable() {
  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags ff = cout.flags();
  cout << hex;

  // Start checking the doc table
  cout << "  checking the doctable..." << endl;
  DocTableChecker dtc(FileDup(file_),
                      kFileIndexHeaderSize,
                      doctableSize_);
  bool docTableGood = dtc.Check(kFileIndexHeaderSize, doctableSize_);

  // set the flags of cout to what they were before we changed them here.
  cout.flags(ff);
  return docTableGood;
}

bool FileIndexChecker::CheckIndexTable() {
  // capture the format flags of cout, so we can set it back to what
  // it was before calling this program
  std::ios_base::fmtflags ff = cout.flags();
  cout << hex;

  // Start checking the index table
  cout << "  checking the index table..." << endl;
  IndexTableChecker itc(FileDup(file_),
                        kFileIndexHeaderSize + doctableSize_,
                        indexSize_);
  bool indexGood = itc.Check(kFileIndexHeaderSize + doctableSize_, indexSize_);
  // set the flags of cout to what they were before we changed them here.
  cout.flags(ff);
  return indexGood;
}

FileIndexChecker::~FileIndexChecker() {
  // Close the (FILE *).
  assert(fclose(file_) == 0);
}

}  // namespace hw3
