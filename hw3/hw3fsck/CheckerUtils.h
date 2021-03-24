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

#ifndef HW3_HW3FSCK_CHECKERUTILS_H_
#define HW3_HW3FSCK_CHECKERUTILS_H_

#include <stdint.h>     // [C++ doesn't yet standardize <cstdint>.]
#include <arpa/inet.h>  // For htonl(), etc.
#include <unistd.h>     // for dup().
#include <cstdio>       // for fdopen(), (FILE *).
#include <string>       // for string.
#include <sstream>       // for stringstream

// This header file declares a set of useful #defines, macros, utility
// functions, and utility classes that are used broadly within HW3.

namespace hw3 {

// These defines are used to avoid hard-coding the length of the file
// header and the offsets of fields within it.
constexpr int kFileIndexHeaderSize = 16;  // size of header
constexpr int kBucketLengthSize = 4;      // size of the field containing the
                                          // number of bucket records
constexpr int kBucketRecordSize = 8;      // size of a single bucket record
constexpr int kElementPositionSize = 4;   // size of an element position in
                                          //  a bucket chain
constexpr int kMagicNumberOffset = 0;     // offset of magic number
constexpr int kWordLenSize = 2;           // size of the field containing the
                                          //  number of bytes in a word
constexpr int kDocIDTableLengthSize = 4;  // size of the field containing the
                                          //  number of bytes in a docid table


// CheckEQ16 checks a 16-bit field within the index file to compare
// its actual value against an expected value.  If they don't match,
// CheckEQ16 prints a message based on fieldname and optional context.
// As well, CheckEQ16 also tests to see if the field is correct but
// swapped endianness, and adapts the error message if so.
// Returns true iff the values are equal, false otherwise.
//
// CheckEQ32 and CheckEQ64 are similar, but for 32-bit and 64-bit fields.
bool CheckEQ16(int16_t expected, int16_t actual,
               const std::string& fieldname);
bool CheckEQ32(int32_t expected, int32_t actual,
               const std::string& fieldname);
bool CheckEQ64(int64_t expected, int64_t actual,
               const std::string& fieldname);

// CheckLT16 checks a 16-bit field within the index file to compare
// its actual value against an expected value.  If expected >= actual,
// CheckLT16 prints a message based on fieldname and optional context.
// Returns true iff bigger >= smaller, false otherwise.
//
// CheckLT32 and CheckLT64 are similar, but for 32-bit and 64-bit fields.
bool CheckLT16(int16_t smaller, int16_t bigger,
               const std::string& fieldname);
bool CheckLT32(int32_t smaller, int32_t bigger,
               const std::string& fieldname);
bool CheckLT64(int64_t smaller, int64_t bigger,
               const std::string& fieldname);

// This helper function uses "varargs" to implement a printf-like interface,
// but for std::string.
std::string ToString(const char* fmt, ...);
}  // namespace hw3

#endif  // HW3_HW3FSCK_CHECKERUTILS_H_
