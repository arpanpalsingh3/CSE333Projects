#include <stdio.h>     // for printf, sscanf

#include "memmove.h" // this pulls over from my h file
#include "string.h" // this is from the library 

#include "c-timer-lib-master/timer.c"

void* memmove_new(void *str1, const void *str2, size_t n) {
    void* copyDest = (void*) str1;
    const void* copySrc =( const void*) str2;

    for(size_t i = 0; i < n; i++) {
        *(char*)(copyDest + i) = *(char*)(copySrc + i);
    }
    return str1;
}



int main()
{

    char src[] = "Hello world I am testing";
    // The destination size based on however long the src is
    int strSize = sizeof(src);
    char destNew[sizeof(src)] = {0};

    
    // copy the number of bytes you want to copy src
    // uses my function
    interval_t * a;

    create_interval(&a, "Test 1", mono, us);
    float aElap = 0;

    int testSize = 10000;

    for (int i = 0; i < testSize; i ++) {

    printf("Running test #%d \n", i+1);

    start(a);
    MEMMOVE(destNew, src, strSize);
    stop(a);

    aElap += elapsed_interval(a,none);

    printf("RAW:\n START: %lld.%.9ld\n END: %lld.%.9ld\n", (long long) a->start.tv_sec, a->start.tv_nsec, (long long) a->stop.tv_sec, a->stop.tv_nsec);
    printf("OUT: %.9f %s\n", elapsed_interval(a, none), print_unit(a->unit));

    }

    aElap = aElap/testSize;

    printf("Copied from: %s \n", src);
    printf("Copied string with new function: %s\n", destNew);

    printf("Average: %.9f %s \n", aElap, "micro seconds");

    return 0;
}