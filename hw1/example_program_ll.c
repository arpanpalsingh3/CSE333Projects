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

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "CSE333.h"
#include "LinkedList.h"

///////////////////////////////////////////////////////////////////////////////
// Prototypes

// The payload type we'll add.
typedef struct {
  int num;
} ExamplePayload;

// Frees an ExamplePayload.
void ExamplePayload_Free(LLPayload_t payload);

// Compator function used for sorting a list of ExamplePayloads.
int ExamplePayload_Comparator(LLPayload_t p1, LLPayload_t p2);


///////////////////////////////////////////////////////////////////////////////
// Main
//
// Here, we demonstrate how to use some of the linked list functions.
int main(int argc, char **argv) {
  ExamplePayload *payload;
  LinkedList *list;
  LLIterator *iter;
  int i;

  // Allocate a list
  list = LinkedList_Allocate();
  Verify333(list != NULL);

  // Insert 100 elements
  for (i = 0; i < 100; i++) {
    payload = (ExamplePayload *) malloc(sizeof(ExamplePayload));
    Verify333(payload != NULL);
    payload->num = i;
    LinkedList_Push(list, (LLPayload_t)payload);

    // Ensure our list total is correct.
    Verify333(LinkedList_NumElements(list) == i+1);
  }

  // Sort the list in descending order.
  LinkedList_Sort(list, false, &ExamplePayload_Comparator);

  // Remove the first element and verify that it was the
  // expected, sorted value.
  Verify333(LinkedList_Pop(list, (LLPayload_t*) &payload));
  Verify333(payload->num == 99);
  Verify333(LinkedList_NumElements(list) == 99);
  free(payload);

  // Make an iterator from the head.
  iter = LLIterator_Allocate(list);
  Verify333(iter != NULL);

  // Peek at the current iterator payload and verify its value.
  LLIterator_Get(iter, (LLPayload_t*) &payload);
  Verify333(payload->num == 98);

  // Move the iterator and verify its value too.
  Verify333(LLIterator_Next(iter));
  LLIterator_Get(iter, (LLPayload_t*) &payload);
  Verify333(payload->num == 97);

  // Clean up after ourselves.
  LLIterator_Free(iter);
  LinkedList_Free(list, &ExamplePayload_Free);
  return 0;
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

void ExamplePayload_Free(LLPayload_t payload) {
  Verify333(payload != NULL);
  free(payload);
}

int ExamplePayload_Comparator(LLPayload_t p1, LLPayload_t p2) {
  int i1, i2;
  Verify333(p1 != NULL);
  Verify333(p2 != NULL);

  i1 = ((ExamplePayload *) p1)->num;
  i2 = ((ExamplePayload *) p2)->num;

  if (i1 > i2)
    return 1;
  if (i1 < i2)
    return -1;
  return 0;
}
