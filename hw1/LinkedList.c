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
#include "LinkedList.h"
#include "LinkedList_priv.h"


///////////////////////////////////////////////////////////////////////////////
// LinkedList implementation.

LinkedList* LinkedList_Allocate(void) {
  // Allocate the linked list record.
  LinkedList *ll = (LinkedList *) malloc(sizeof(LinkedList));
  Verify333(ll != NULL);

  // STEP 1: initialize the newly allocated record structure.
  // ll starts off empty 
  // points to nothing at the start
  ll->head = NULL;
  ll->tail = NULL;
  // has no elements
  ll->num_elements = 0;

  // Return it
  return ll;
}

void LinkedList_Free(LinkedList *list,
                     LLPayloadFreeFnPtr payload_free_function) {
  Verify333(list != NULL);
  Verify333(payload_free_function != NULL);

  // STEP 2: sweep through the list and free all of the nodes' payloads
  // (using the payload_free_function supplied as an argument) and
  // the nodes themselves.

  // go through the whole list
   while (list->head != NULL) {
    // free the payload
    payload_free_function(list->head->payload);
    // pointer to hold node head so we can free still after moving head
    LinkedListNode* temp = list->head;
    list->head = list->head->next;
    free(temp);
  }
  // we free the list itself as well
  free(list);
  
}

int LinkedList_NumElements(LinkedList *list) {
  Verify333(list != NULL);
  return list->num_elements;
}

void LinkedList_Push(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // Allocate space for the new node.
  LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
  Verify333(ln != NULL);

  // Set the payload
  ln->payload = payload;

  if (list->num_elements == 0) {
    // Degenerate case; list is currently empty
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);

    ln->next = NULL;
    ln->prev = NULL;
    
    list->head = list->tail = ln;
    list->num_elements = 1;
  } else {
    // STEP 3: typical case; list has >=1 elements
    Verify333(list->head != NULL);
    Verify333(list->tail != NULL);

    ln->prev = NULL;
    ln->next = list->head;

    list->head->prev = ln;
    list->head = ln;

    list->num_elements += 1;
  }
}

bool LinkedList_Pop(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 4: implement LinkedList_Pop.  Make sure you test for
  // and empty list and fail.  If the list is non-empty, there
  // are two cases to consider: (a) a list with a single element in it
  // and (b) the general case of a list with >=2 elements in it.
  // Be sure to call free() to deallocate the memory that was
  // previously allocated by LinkedList_Push().

  // nothing to be poped from the empty list, so return false
  if (list->num_elements == 0)
    return false;

  // note the payload in return parameter
  *payload_ptr = list->head->payload;
  // hold list head in temp to free later
  LinkedListNode* temp = list->head;

  if (list->num_elements == 1) {
    // single node list, return to empty list values
    list->head = NULL;
    list->tail = NULL;

  } else {
    // multiple node list, move the head
    list->head = list->head->next;
    list->head->prev = NULL;
  }
  // decrement the elements due to removing an element
  list->num_elements -= 1;
  // free the old head 
  free(temp);
  return true;
}

void LinkedList_Append(LinkedList *list, LLPayload_t payload) {
  Verify333(list != NULL);

  // STEP 5: implement LinkedList_Append.  It's kind of like
  // LinkedList_Push, but obviously you need to add to the end
  // instead of the beginning.
   LinkedListNode *ln = (LinkedListNode *) malloc(sizeof(LinkedListNode));
   Verify333(ln != NULL);

   ln->payload = payload;

  // if list is empty
  if (list->num_elements == 0) {
    Verify333(list->head == NULL);
    Verify333(list->tail == NULL);
    // the nodes point to nothng since they're the only ones in the list
    ln->next = NULL;
    ln->prev = NULL;
    // the list points to only the node (its the front and the back)
    list->head = ln;
    list->tail = ln;

    list->num_elements = 1;

  } else { // when list is not empty
    Verify333(list->head != NULL);
    Verify333(list->tail != NULL);
    // nothing is after the new addition
    ln->next = NULL;
    // the old tail is now the pre of new addition
    ln->prev = list->tail;
    // the tail is updated 
    list->tail->next = ln;
    list->tail = ln;

    list->num_elements += 1;
  }
}

void LinkedList_Sort(LinkedList *list, bool ascending,
                     LLPayloadComparatorFnPtr comparator_function) {
  Verify333(list != NULL);
  if (list->num_elements < 2) {
    // No sorting needed.
    return;
  }

  // We'll implement bubblesort! Nnice and easy, and nice and slow :)
  int swapped;
  do {
    LinkedListNode *curnode;

    swapped = 0;
    curnode = list->head;
    while (curnode->next != NULL) {
      int compare_result = comparator_function(curnode->payload,
                                               curnode->next->payload);
      if (ascending) {
        compare_result *= -1;
      }
      if (compare_result < 0) {
        // Bubble-swap the payloads.
        LLPayload_t tmp;
        tmp = curnode->payload;
        curnode->payload = curnode->next->payload;
        curnode->next->payload = tmp;
        swapped = 1;
      }
      curnode = curnode->next;
    }
  } while (swapped);
}


///////////////////////////////////////////////////////////////////////////////
// LLIterator implementation.

LLIterator* LLIterator_Allocate(LinkedList *list) {
  Verify333(list != NULL);

  // OK, let's manufacture an iterator.
  LLIterator *li = (LLIterator *) malloc(sizeof(LLIterator));
  Verify333(li != NULL);

  // Set up the iterator.
  li->list = list;
  li->node = list->head;

  return li;
}

void LLIterator_Free(LLIterator *iter) {
  Verify333(iter != NULL);
  free(iter);
}

bool LLIterator_IsValid(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);

  return (iter->node != NULL);
}

bool LLIterator_Next(LLIterator *iter) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 6: try to advance iterator to the next node and return true if
  // you succeed, false otherwise
  
  // if true, there is a next, if false , there is no next
  bool iterHasNext = !(iter->node->next == NULL);

  if (iterHasNext) {
    // move the iterator forward and return true
    iter->node = iter->node->next;
    return true;
  }
  // There isn't another node, so return failure.
  iter->node = NULL;
  return false;
}

void LLIterator_Get(LLIterator *iter, LLPayload_t *payload) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  *payload = iter->node->payload;
}

bool LLIterator_Remove(LLIterator *iter,
                       LLPayloadFreeFnPtr payload_free_function) {
  Verify333(iter != NULL);
  Verify333(iter->list != NULL);
  Verify333(iter->node != NULL);

  // STEP 7: implement LLIterator_Remove.  This is the most
  // complex function you'll build.  There are several cases
  // to consider:
  // - degenerate case: the list becomes empty after deleting.
  // - degenerate case: iter points at head
  // - degenerate case: iter points at tail
  // - fully general case: iter points in the middle of a list,
  //                       and you have to "splice".
  //
  // Be sure to call the payload_free_function to free the payload
  // the iterator is pointing to, and also free any LinkedList
  // data structure element as appropriate.

  
  // free the current node payload
  payload_free_function(iter->node->payload);
  // note current note in temp to free later
  LinkedListNode* temp = iter->node;

  if (iter->list->num_elements == 1) {
    // list becomes empty, set everything to null
    iter->node = NULL;

    iter->list->head = NULL;
    iter->list->tail = NULL;

    iter->list->num_elements -= 1;
    // free the old node
    free(temp);
    // return false since the list is empty now
    return false;
  }

  if (iter->node->prev == NULL) {
    // when iterator points at the head of the list
    // we move the head 
    iter->node = iter->node->next;
    iter->list->head = iter->node;
    iter->node->prev = NULL;
  } else if (iter->node->next == NULL) {
    // when iterator points at the tail of the list
    // we move the tail
    iter->node = iter->node->prev;
    iter->list->tail = iter->node;
    iter->node->next = NULL;
  } else {
    // iterator points at the middle of the list
    // update the next of the prev of this node
    iter->node->next->prev = iter->node->prev;
    // update the prev of the next of this node
    iter->node->prev->next = iter->node->next;
    // update the node
    iter->node = iter->node->next;
  }
  // decrement the num elements since something got removed
  iter->list->num_elements -= 1;
  // free the temp node we allocated at the start and return 
  // true since it was a succesful remove and the list is still
  // non empty
  free(temp);
  return true;
}


///////////////////////////////////////////////////////////////////////////////
// Helper functions

bool LinkedList_Slice(LinkedList *list, LLPayload_t *payload_ptr) {
  Verify333(payload_ptr != NULL);
  Verify333(list != NULL);

  // STEP 8: implement LinkedList_Slice.

  // list empty, nothing to remove
  if (list->num_elements == 0)
    return false;

  // note payload in return parameter
  *payload_ptr = list->tail->payload;
  // pointer to the tail of the list
  LinkedListNode* temp = list->tail;

  if (list->num_elements == 1) {
    // list only has one node, so remove returns to all nulls
    list->head = NULL;
    list->tail = NULL;
  } else {
    // list has at least two nodes, so update tail since its a slice
    list->tail = list->tail->prev;
    list->tail->next = NULL;
  }
  // decrement elements since we are removing one
  list->num_elements -= 1;
  // free the temp node (old tail)
  free(temp);
  return true;
}

void LLIterator_Rewind(LLIterator *iter) {
  iter->node = iter->list->head;
}
