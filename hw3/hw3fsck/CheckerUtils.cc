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

#include "./CheckerUtils.h"

#include <cstdarg>
#include <iostream>  // for std::cout / std::endl
#include <string>

#include "../Utils.h"

using std::cout;
using std::endl;
using std::hex;
using std::string;

namespace hw3 {

bool CheckEQ16(int16_t expected, int16_t actual, const string& fieldname) {
  if (expected == actual) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << expected;
  cout << ", but actually is " << actual << ".";
  if (ntohs(actual) == expected) {
    cout << "  Note: you likely ";
    cout << "forgot an endianness conversion, since ntohs(actual) ";
    cout << "== expected.";
  }
  cout << endl;
  return false;
}

bool CheckEQ32(int32_t expected, int32_t actual, const string& fieldname) {
  if (expected == actual) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << expected;
  cout << ", but actually is " << actual << ".";
  if ((signed)ntohl(actual) == expected) {
    cout << "  Note: you likely ";
    cout << "forgot an endianness conversion, since ntohl(actual) ";
    cout << "== expected.";
  }
  cout << endl;
  return false;
}

bool CheckEQ64(int64_t expected, int64_t actual, const string& fieldname) {
  if (expected == actual) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << expected;
  cout << ", but actually is " << actual << ".";
  if ((signed)ntohll(actual) == expected) {
    cout << "  Note: you likely ";
    cout << "forgot an endianness conversion, since ntohll(actual) ";
    cout << "== expected.";
  }
  cout << endl;
  return false;
}

bool CheckLT16(int16_t smaller, int16_t bigger, const string& fieldname) {
  if (smaller < bigger) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << smaller;
  cout << " < " << bigger << ".";
  if (ntohs(smaller) < bigger) {
    cout << "  Note: there is a chance that you ";
    cout << "forgot an endianness conversion, since ntohs(" << smaller;
    cout << ") = " << ntohs(smaller) << " < " << bigger << ".";
  }
  cout << endl;
  return false;
}

bool CheckLT32(int32_t smaller, int32_t bigger, const string& fieldname) {
  if (smaller < bigger) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << smaller;
  cout << " < " << bigger << ".";
  if ((signed)ntohl(smaller) < bigger) {
    cout << "  Note: it's possible that you ";
    cout << "forgot an endianness conversion, since ntohl(" << smaller;
    cout << ") = " << ntohl(smaller) << " < " << bigger << ".";
  }
  cout << endl;
  return false;
}

bool CheckLT64(int64_t smaller, int64_t bigger, const string& fieldname) {
  if (smaller < bigger) {
    return true;
  }

  cout << hex;
  cout << fieldname << ": expected " << smaller;
  cout << " < " << bigger << ".";
  if ((signed)ntohll(smaller) < bigger) {
    cout << "  Note: it's possible that you ";
    cout << "forgot an endianness conversion, since ntohll(" << smaller;
    cout << ") = " << ntohll(smaller) << " < " << bigger << ".";
  }
  cout << endl;
  return false;
}

string ToString(const char* fmt, ...) {
  constexpr int kBufSize = 512;
  char buf[kBufSize];

  // Initialize the varargs.
  va_list args;
  va_start(args, fmt);

  // Dispatch to the varargs-aware version of printf.
  vsnprintf(buf, kBufSize, fmt, args);

  // Clean up varargs.
  va_end(args);

  return string(buf);
}

}  // namespace hw3
