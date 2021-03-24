/*
Name: Arpanpal Singh
Email: arpans@uw.edu
CSE-Email: arpans@cs.washington.edu 
*/

#include <stdio.h>     // for printf, sscanf
#include <stdint.h>    // for int8_t, etc.
#include <inttypes.h>  // for extended integer print formats
#include <stdlib.h>    // for EXIT_SUCCESS / EXIT_FAILURE and exit

#include "dumphex.h"

int DumpHex(void *mem_addr, uint32_t num_bytes) {
    uint8_t* ptr = mem_addr;

    if (num_bytes < 0 ) {
        printf("unrealisitc size");
        return 1; 
    }

    // The print statement 
    printf("The %d bytes starting at %p are:", num_bytes, mem_addr);

    // Looping through the pointer to get each byte 
    for (int i = 0; i < num_bytes; i++) {
        printf("%s %02" PRIx8, (i > 0 ? " " : ""), ptr[i]);
    }
    // New line for next read
    printf("\n");
    
    return 0; 
}
