// Name:  Arpanpal Singh
// Email: arpans@uw.edu
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

#include <stdio.h>     // for printf, sscanf
#include <stdint.h>    // for int8_t, etc.
#include <inttypes.h>  // for extended integer print formats
#include <stdlib.h>    // for EXIT_SUCCESS / EXIT_FAILURE and exit

#include "ex01.h"      // defines symbol for array initial data

/************************************************************
 * This is a very skeletal skeleton.
 * Lines starting "// ++++" are messages trying to indicate what 
 * you should implement.
 *************************************************************/

int main(int argc, char **argv) {
  // Create the unsorted array
  // ++++ The array's size will be determined by the initialization data,
  // ++++ which has been put in a separate file to make it easy to change.
  // ++++ Change it.
  int8_t unsorted[] = INIT_DATA;  // see contents of ex01.h

  // ++++ Initialize ARRAY_SIZE to the number of elements in array unsorted.
  // ++++ Hint: use sizeof(), twice
  // ++++ Note: It doesn't make sense for code to change ARRAY_SIZE once
  // ++++ initialized.  The "const" in the declaration makes it harder
  // ++++ for some programmer modifying this code in the future to make
  // ++++ accidentially decided to change it.  It's kind of like "final"
  // ++++ in Java, except it's possible to get around it if you try.
  const uint32_t ARRAY_SIZE = sizeof(unsorted);

  // create the second array
  int8_t sorted[ARRAY_SIZE];

  // invoke CopyAndSort()
  if ( CopyAndSort(unsorted, sorted, ARRAY_SIZE) > 0 ) {
    fprintf(stderr, "CopyAndSort() returned failure\n");
    return EXIT_FAILURE;
  }

  // ++++ Print the sorted (second) array.
  for (int i = 0; i < ARRAY_SIZE; i++) {
      printf(" %d " , sorted[i]);
  } printf("\n");
  // All done
  return EXIT_SUCCESS;
}

int CopyAndSort(int8_t* start, int8_t* end, uint32_t size) {
  if (size <= 0) {
    printf("Invalid array size, return value 1\n");
    return 1;
  }
  if (start == NULL || end == NULL) {
    printf("Null array, return value 2\n");
    return 2;
  }
  if (sizeof(start) != sizeof(end)) {
    printf("Mismatching arrays, return value 3\n");
    return 3;
  }

  // Copy value in 0 index
  end[0] = start[0];

  for (int i = 1; i < size; i++) {
    // Go through start array and copy over
    end[i] = start[i];
    // check if copied value breaks sort
    if (end[i] < end[i-1]) {
      // if it does, create another forloop at that point going backwards
      for (int j = i; j > 0; j--) {
        if (end[j-1] < end[j]) {
          break;
        }
        // swapping [j] with [j-1]
        int temp = end[j-1];
        end[j-1] = end[j];
        end[j] = temp;
      }
    }
  }
  return 0;
}
