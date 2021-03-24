/*
 * Copyright ©2021 John Zahorjan.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Winter Quarter 2021 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include <stdio.h>       // for printf
#include <stdlib.h>      // for EXIT_SUCCESS
#include <inttypes.h>    // for uint8_t, PRIx8, etc.

#include "../dumphex.h"

// TEST(x) is called a preprocessor macro.
// It looks like a procedure call.  It acts like one EXCEPT
// that it is invoked at preprocess time, before compilation.
// The argument(s) is a string.  Whatever is passed as the arg
// takes its place when expanding the symbol TEST in the
// C program.  Here the arguments name is x.  #x means "put quotes
// around whatever string was passed as x."  In C you can
// concatenate literal strings just by putting them next to each other.
// So, "one two" and "one " "two" are the same literal.
#define TEST(x)   if ( DumpHex(&x , sizeof(x)) ) \
                      fprintf(stderr, "DumpHex() of " #x " failed\n");


int main(int argc, char **argv) {
  typedef struct {
    char    char_val;
    int32_t int_val;
    float   float_val;
    double  double_val;
  } Ex02Struct_t;         // '"Ex02Struct_t" is now a synonym for
                          // the struct type listed
  Ex02Struct_t struct_val = { '0', 1, 1.0, 1.0 };

  // This is why I wanted to define the macro:
  // (1) to avoid the tedium of typing the full expansion out over and over, and
  // (2) most importantly, so there'd be one "definition" of the test call so
  //     thatif I want to change it later I go to one place (the #define above)
  //     rather than having to change it in 5 distinct statements, which is
  //     more error prone
  TEST(struct_val.char_val);
  TEST(struct_val.int_val);
  TEST(struct_val.float_val);
  TEST(struct_val.double_val);
  TEST(struct_val);

  return EXIT_SUCCESS;
}
