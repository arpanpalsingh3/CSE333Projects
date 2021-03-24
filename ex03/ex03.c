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

#include <stdio.h>    // for printf
#include <stdlib.h>   // for EXIT_SUCCESS, malloc, free
#include <assert.h>   // for assert()

#include "vector3d.h"

const int TESTX = 1;
const int TESTY = 22;
const int TESTZ = 333;

/******************************************************************
 * All this mainline does is test that Vector3dNew and Vector3dAdd
 * don't obviously fail to work.
 ******************************************************************/
int main(int argc, char **argv) {
  int return_code = EXIT_FAILURE;  // lots of ways to fail,
                                   // only one way to succeed

  // REMEMBER: A Vector3d is a pointer!
  Vector3d vec_one = Vector3d_new(TESTX, TESTY, TESTZ);
  if (vec_one == NULL) fprintf(stderr, "Vector3d_new() of vec_one failed.\n");
  else {
    // vec_one has been created
    Vector3d vec_two = Vector3d_new(2*TESTX, -2*TESTY, 0);
    if (vec_two == NULL) fprintf(stderr, "Vector3d_new() of vec_two failed.\n");
    else {
      // vec_one and vec_two have been created
      Vector3d vec_sum = Vector3d_add(vec_one, vec_two);
      if (vec_sum == NULL) fprintf(stderr, "Vector3d_add() failed.\n");
      else {
        // Do vector assignment
	Vector3d_destroy(vec_one);
	vec_one = vec_two;
	// Modify one of two vectors involved in assignment
	Vector3d_setZ( vec_one, -100);

	// Print the three
	Vector3d_print(vec_one, stdout);  printf("\n");
	Vector3d_print(vec_two, stdout);  printf("\n");
	Vector3d_print(vec_sum, stdout);  printf("\n");
	
	// Verify results are correct.
        // assert() is used ONLY during code development / debug
        assert(Vector3d_getX(vec_one) == 2*TESTX);
        assert(Vector3d_getY(vec_one) == -2*TESTY);
        assert(Vector3d_getZ(vec_one) == -100);

	assert(Vector3d_getX(vec_two) == 2*TESTX);
        assert(Vector3d_getY(vec_two) == -2*TESTY);
        assert(Vector3d_getZ(vec_two) == -100);

	assert(Vector3d_getX(vec_sum) == 3*TESTX);
        assert(Vector3d_getY(vec_sum) == -TESTY);
        assert(Vector3d_getZ(vec_sum) == TESTZ);

        // PASSED!
        return_code = EXIT_SUCCESS;

        Vector3d_destroy(vec_sum);
	vec_one = NULL;
      }
      Vector3d_destroy(vec_two);
    }
    Vector3d_destroy(vec_one);
  }

  return return_code;
}
