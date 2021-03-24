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

#ifndef HW3_HW3FSCK_FILEINDEXCHECKER_H_
#define HW3_HW3FSCK_FILEINDEXCHECKER_H_

#include <stdint.h>  // for uint32_t, etc.
#include <string>    // for std::string
#include <cstdio>    // for (FILE *)

#include "../Utils.h"

namespace hw3 {

// A FileIndexChecker is used to access an index file, validate the
// header information out of it, and manufacture DocTableChecker and
// IndexTableChecker hash table accessors.  (To manufacture a
// DocIDTableChecker, you use a manufactured IndexTableChecker.)
class FileIndexChecker {
 public:
  // Arguments:
  //
  // - filename: the index file to load.
  explicit FileIndexChecker(const std::string &filename);
  ~FileIndexChecker();

  // Checks the entire file that this FileIndexChecker is constructed with.
  // returns false iff any errors were noticed in the index file.
  // returns true otherwise.
  bool CheckFile();

  // Checks the header of the index file this is managing.
  // Returns false iff any errors were noticed in the header
  // of the index file. Returns true otherwise.
  bool CheckHeader();

  // Checks the doc table of the index file this is managing.
  // returns false iff any errors were noticed in the doctable
  // of the index file. Returns true otherwise.
  bool CheckDocTable();

  // Checks the index table of the index file this is managing.
  // Returns false iff any errors were noticed in the index table of
  // the index file. Returns true otherwise.
  bool CheckIndexTable();

 private:
  // The name of the index file we're checking.
  std::string filename_;

  // The stdio.h (FILE *) for the file.
  FILE *file_;

  // A cached copy of the doctable size field from the index header.
  int32_t doctableSize_;

  // A cached copy of the index size field from the index header.
  int32_t indexSize_;

  DISALLOW_COPY_AND_ASSIGN(FileIndexChecker);
};

}  // namespace hw3

#endif  // HW3_HW3FSCK_FILEINDEXCHECKER_H_
