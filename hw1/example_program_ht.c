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

#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "HashTable.h"

///////////////////////////////////////////////////////////////////////////////
// Prototypes

// Our payload.
typedef struct {
  int num;
} ExampleValue;

// A function for freeing our payload.
void ExampleValue_Free(HTValue_t value);



///////////////////////////////////////////////////////////////////////////////
// Main
//
// Here, we demonstrate how to use some of the hash table functions.
int main(int argc, char **argv) {
  ExampleValue *evp;
  HashTable *ht;
  HTIterator *iter;
  HTKeyValue_t kv, old_kv;
  HTKey_t i;

  // Allocate a hash table with 10,000 initial buckets
  ht = HashTable_Allocate(10000);
  Verify333(ht != NULL);

  // Insert 20,000 elements (load factor = 2.0).
  for (i = 0; i < 20000; i++) {
    evp = (ExampleValue *) malloc(sizeof(ExampleValue));
    Verify333(evp != NULL);
    evp->num = i;

    // Ensure the HT has the expected number of elements before and after
    // performing the insert.
    Verify333(HashTable_NumElements(ht) == i);

    // Insert the new element
    kv.key = i;
    kv.value = (HTValue_t)evp;
    Verify333(!HashTable_Insert(ht, kv, &old_kv));

    Verify333(HashTable_NumElements(ht) == i+1);
  }

  // Look up a few values.
  Verify333(HashTable_Find(ht, (HTKey_t)100, &kv));
  Verify333(kv.key == 100);
  Verify333(((ExampleValue *) kv.value)->num == 100);

  Verify333(HashTable_Find(ht, (HTKey_t)18583, &kv));
  Verify333(kv.key == 18583);
  Verify333(((ExampleValue *) kv.value)->num == 18583);

  // Ensure non-existent values cannot be found.
  Verify333(!HashTable_Find(ht, (HTKey_t)20000, &kv));

  // Delete a value that's in the table.
  Verify333(HashTable_Remove(ht, (HTKey_t)100, &kv));
  Verify333(kv.key == (HTKey_t)100);
  Verify333(((ExampleValue *) kv.value)->num == 100);
  ExampleValue_Free(kv.value);   // since we malloc'ed it, we must free it

  // Ensure it's deleted.
  Verify333(!HashTable_Find(ht, (HTKey_t)100, &kv));
  Verify333(HashTable_NumElements(ht) == 19999);

  // Loop through using an iterator.
  i = 0;
  iter = HTIterator_Allocate(ht);
  Verify333(iter != NULL);

  while (HTIterator_IsValid(iter)) {
    Verify333(HTIterator_Get(iter, &kv));
    Verify333(kv.key != (HTKey_t)100);   // shouldn't be there; we deleted it

    // Advance the iterator and counter.
    HTIterator_Next(iter);
    i++;
  }
  Verify333(i == 19999);

  // free the iterator

  // Clean up.
  HTIterator_Free(iter);
  HashTable_Free(ht, &ExampleValue_Free);
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

void ExampleValue_Free(HTValue_t value) {
  Verify333(value != NULL);
  free(value);
}
