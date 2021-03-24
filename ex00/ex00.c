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

#include <stdio.h>   // for printf, sscanf
#include <stdlib.h>  // for EXIT_SUCCESS / EXIT_FAILURE and exit
#include <stdint.h>  // for uint32_t
#include <string.h>  // for strtol

// Print out the usage of the program
void Usage(void);

// Return the nth term in the Nilakantha series.
double NthNilakantha_term(uint32_t n) { 
  int32_t plusMinus = -1; 
  double result = 3.0; // represents n = 0 

  for (uint32_t i = 1; i <= n; i++) {
       plusMinus *= -1; // flipping back and forth between adding and subtracting 
       
       // the following is the actual formula from instructions 
       result += plusMinus * (4.0 / ((2.0 * i) * (2.0 * i + 1) * (2.0 * i + 2)));
    }
    return result;
}

int main(int argc, char **argv) {
  // Make sure the user provided us with a single command line argument
  // and exit if not.
  if (argc != 2) {
    Usage();
    exit(EXIT_FAILURE);
  }

  // Try to convert the command line argument to an integer >= 0
  // and exit if unable.
  int num_terms; // Keeping track of how many different types I have
  char catch; // To catch extra characters 

  int num_items = sscanf(argv[1], "%d %c" , &num_terms, &catch);
  
  if (num_items != 1 || num_terms < 0) { // if I catch more than 1 type (integers), its bad input
    Usage();
    exit(EXIT_FAILURE);
  }
  
  // Calculate and print the estimate.
  double estimate = NthNilakantha_term(num_terms);

  printf("Our estimate of Pi is %0.20f\n", estimate);

  // Quit
  return EXIT_SUCCESS;
}

void Usage(void) {
  fprintf(stderr,
          "usage: ./ex0 n, where n>=0.  Prints pi estimated " \
          "to n terms of the Nilakantha series.\n");
}
