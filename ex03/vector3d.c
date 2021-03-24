#include <stdio.h>     // for printf, sscanf
#include <stdint.h>    // for int8_t, etc.
#include <inttypes.h>  // for extended integer print formats
#include <stdlib.h>    // for EXIT_SUCCESS / EXIT_FAILURE and exit

#include "vector3d.h"

typedef struct Vector3d {
    int x;
    int y;
    int z;
} *Vector3d;

Vector3d Vector3d_new(int x, int y, int z) {
    Vector3d newVec = malloc(sizeof(int)*3);
    newVec->x = x;
    newVec->y = y;
    newVec->z = z;
    
    return newVec;
}

void Vector3d_destroy(Vector3d v) {
    free(v);
}

int Vector3d_getX(Vector3d v) {
    return v->x;
}  // returns x component of v
int Vector3d_getY(Vector3d v) {
    return v->y;
}  // returns y component of v
int Vector3d_getZ(Vector3d v) {
    return v->z;
}  // returns z component of v

Vector3d Vector3d_setX(Vector3d v, int x) {
    v->x = x;
    return v;
}
Vector3d Vector3d_setY(Vector3d v, int y) {
    v->y = y;
    return v;
}
Vector3d Vector3d_setZ(Vector3d v, int z) {
    v->z = z;
    return v;
}

Vector3d Vector3d_add(Vector3d vec_one, Vector3d vec_two) {
    int newx = vec_one->x + vec_two->x;
    int newy = vec_one->y + vec_two->y;
    int newz = vec_one->z + vec_two->z;
    Vector3d newVec = Vector3d_new(newx, newy, newz);
    return newVec;
}

void Vector3d_print(Vector3d v, FILE* f_out) {
    fprintf(f_out, "[%d, %d, %d]", v->x, v->y, v->z);
}
