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

#include "dumphex.h"

int main(int argc, char **argv) {
  typedef struct {
    char    char_val;
    int32_t int_val;
    float   float_val;
    double  double_val;
  } Ex02Struct_t;         // '"Ex02Struct_t" is now a synonym for
                          // the struct type listed
  Ex02Struct_t struct_val = { '0', 1, 1.0, 1.0 };

  if ( DumpHex(&struct_val.char_val, sizeof(char)) )
    fprintf(stderr, "DumpHex() of char failed\n");
  if ( DumpHex(&struct_val.int_val, sizeof(int32_t)) )
    fprintf(stderr, "DumpHex() of int32_t failed\n");
  if ( DumpHex(&struct_val.float_val, sizeof(float)) )
    fprintf(stderr, "DumpHex() of float failed\n");
  if ( DumpHex(&struct_val.double_val, sizeof(double)) )
    fprintf(stderr, "DumpHex() of double failed\n");
  if ( DumpHex(&struct_val, sizeof(struct_val)) )
    fprintf(stderr, "DumpHex() of struct failed\n");

  return EXIT_SUCCESS;
}
