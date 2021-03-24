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
#ifndef VECTOR3D_H_
#define VECTOR3D_H_

#include <stdio.h>

// Define the Vector3d type as _______________________
// Try to make it hard for client code to look inside your Vector3d.
// (You want "to force" them to use your getter/setter methods.)
typedef struct Vector3d *Vector3d;

// Create a new Vector3d
//   Returns a new Vector3d, or NULL for error
Vector3d Vector3d_new(int x, int y, int z);

// Destroy a Vector3d.  Client must call once for each new'ed Vector3d.
void Vector3d_destroy(Vector3d v);

// Getter functions operating on parameter v.  (There is no way to indicate failure,
// but you've got to return something.  Return 0.)
int Vector3d_getX(Vector3d v);  // returns x component of v
int Vector3d_getY(Vector3d v);  // returns y component of v
int Vector3d_getZ(Vector3d v);  // returns z component of v

// Setter functions operating on parameter v.  All return parameter v,
// or something sensible for error.
Vector3d Vector3d_setX(Vector3d v, int x);
Vector3d Vector3d_setY(Vector3d v, int y);
Vector3d Vector3d_setZ(Vector3d v, int z);

// Returns a new vector that is the component-wise sum of the two argument vectors.
// Returns something sensible for error.
Vector3d Vector3d_add(Vector3d vec_one, Vector3d vec_two);

// Prints the vector's components to f_out (use fprintf() rather than printf())
void Vector3d_print(Vector3d v, FILE* f_out);

#endif  // VECTOR3D_H_
