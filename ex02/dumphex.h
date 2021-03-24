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

#ifndef DUMPHEX_H_
#define DUMPHEX_H_

// This file includes inttypes.h (for uint32_t) even though
// we know the client file must also include it so that the
// client can include this file and <inttypes.h> in either order.
// If we didn't include it and then if the client included this
// file before <inttypes.h> an error would be raised by the
// declaration of DumpHex in this file.
#include <inttypes.h>

int DumpHex(void *mem_addr, uint32_t num_bytes);

#endif  // DUMPHEX_H_
