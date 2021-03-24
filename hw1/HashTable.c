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
#include <stdint.h>

#include "CSE333.h"
#include "HashTable.h"
#include "HashTable_priv.h"

///////////////////////////////////////////////////////////////////////////////
// Internal helper functions.
//
#define INVALID_IDX -1

// Grows the hashtable (ie, increase the number of buckets) if its load
// factor has become too high.
static void MaybeResize(HashTable *ht);

// helper
static bool hasKey(LLIterator *lliter, int key, HTKeyValue_t **keyvalue);

int HashKeyToBucketNum(HashTable *ht, HTKey_t key) {
  return key % ht->num_buckets;
}

// Deallocation functions that do nothing.  Useful if we want to deallocate
// the structure (eg, the linked list) without deallocating its elements or
// if we know that the structure is empty.
static void LLNoOpFree(LLPayload_t freeme) { }
static void HTNoOpFree(HTValue_t freeme) { }


///////////////////////////////////////////////////////////////////////////////
// HashTable implementation.

HTKey_t FNVHash64(unsigned char *buffer, int len) {
  // This code is adapted from code by Landon Curt Noll
  // and Bonelli Nicola:
  //     http://code.google.com/p/nicola-bonelli-repo/
  static const uint64_t FNV1_64_INIT = 0xcbf29ce484222325ULL;
  static const uint64_t FNV_64_PRIME = 0x100000001b3ULL;
  unsigned char *bp = (unsigned char *) buffer;
  unsigned char *be = bp + len;
  uint64_t hval = FNV1_64_INIT;

  // FNV-1a hash each octet of the buffer.
  while (bp < be) {
    // XOR the bottom with the current octet.
    hval ^= (uint64_t) * bp++;
    // Multiply by the 64 bit FNV magic prime mod 2^64.
    hval *= FNV_64_PRIME;
  }
  return hval;
}

HashTable* HashTable_Allocate(int num_buckets) {
  HashTable *ht;
  int i;

  Verify333(num_buckets > 0);

  // Allocate the hash table record.
  ht = (HashTable *) malloc(sizeof(HashTable));

  // Initialize the record.
  ht->num_buckets = num_buckets;
  ht->num_elements = 0;
  ht->buckets = (LinkedList **) malloc(num_buckets * sizeof(LinkedList *));
  for (i = 0; i < num_buckets; i++) {
    ht->buckets[i] = LinkedList_Allocate();
  }

  return ht;
}

void HashTable_Free(HashTable *table,
                    ValueFreeFnPtr value_free_function) {
  int i;

  Verify333(table != NULL);

  // Free each bucket's chain.
  for (i = 0; i < table->num_buckets; i++) {
    LinkedList *bucket = table->buckets[i];
    HTKeyValue_t *kv;

    // Pop elements off the chain list one at a time.  We can't do a single
    // call to LinkedList_Free since we need to use the passed-in
    // value_free_function -- which takes a HTValue_t, not an LLPayload_t -- to
    // free the caller's memory.
    while (LinkedList_NumElements(bucket) > 0) {
      Verify333(LinkedList_Pop(bucket, (LLPayload_t *)&kv));
      value_free_function(kv->value);
      free(kv);
    }
    // The chain is empty, so we can pass in the
    // null free function to LinkedList_Free.
    LinkedList_Free(bucket, LLNoOpFree);
  }

  // Free the bucket array within the table, then free the table record itself.
  free(table->buckets);
  free(table);
}

int HashTable_NumElements(HashTable *table) {
  Verify333(table != NULL);
  return table->num_elements;
}

bool HashTable_Insert(HashTable *table,
                      HTKeyValue_t newkeyvalue,
                      HTKeyValue_t *oldkeyvalue) {
  int bucket;
  LinkedList *chain;

  Verify333(table != NULL);
  MaybeResize(table);

  // Calculate which bucket and chain we're inserting into by using helper
  // which turns keyvalues into integer bucket numbers
  bucket = HashKeyToBucketNum(table, newkeyvalue.key);
  // go into the said bucket and access the linked list there
  chain = table->buckets[bucket];

  // STEP 1: finish the implementation of InsertHashTable.
  // This is a fairly complex task, so you might decide you want
  // to define/implement a helper function that helps you find
  // and optionally remove a key within a chain, rather than putting
  // all that logic inside here.  You might also find that your helper
  // can be reused in steps 2 and 3.

  // allocate space for new key/value
  HTKeyValue_t *newInsert = (HTKeyValue_t *) malloc(sizeof(HTKeyValue_t));
  Verify333(newInsert != NULL);

  //initialize its values to be what we want
  newInsert->key = newkeyvalue.key;
  newInsert->value = newkeyvalue.value;
  
  // base case of when the chain is empty (the list is empty)
  if (LinkedList_NumElements(chain) == 0) {
    // push newinsert to the bucket, return false since empty chain
    LinkedList_Push(chain, (void *) newInsert);
    // increment table since new element added
    table->num_elements += 1;
    return false;
  }

  // # For when the chain is not empty 

  HTKeyValue_t *oldKeyValPair; // to store old values
  // iterator to go through the chain 
  LLIterator *lliter = LLIterator_Allocate(chain);

  // Check if the key is present
  if (hasKey(lliter, newkeyvalue.key, &oldKeyValPair)) {
    // append new key/value to the bucket
    LinkedList_Append(chain, (void *) newInsert);

    // take note of the old key/val to put in return parameter
    // by using the return parameter of the hasKey function
    oldkeyvalue->key = oldKeyValPair->key;
    oldkeyvalue->value = oldKeyValPair->value;
    // delete oldkeyvalue from the bucket (which is where the iterator is at right now)
    LLIterator_Remove(lliter, &LLNoOpFree);
    // free the allocated valpair since we malloc when we inserted it
    free(oldKeyValPair);
    LLIterator_Free(lliter);
    // no need to increment table size since we are replacing
    return true;

  } else { // case when the key isn't already present

    // append new key val pair 
    LinkedList_Append(chain, (void *) newInsert);
    // new addition, so increment table size
    table->num_elements += 1;
    LLIterator_Free(lliter);
    // return false since there was no matching key
    return false;
  }
} 

bool HashTable_Find(HashTable *table,
                    HTKey_t key,
                    HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 2: implement HashTable_Find.
  int bucket;
  LinkedList *chain;

  // Calculate which bucket and chain we're inserting into by using helper
  // which turns keyvalues into integer bucket numbers
  bucket = HashKeyToBucketNum(table, key);
  // go into the said bucket and access the linked list there
  chain = table->buckets[bucket];

  // if the buck is empty, nothing to find, so search is false
  if (LinkedList_NumElements(chain) == 0) {
    return false;
  }

  LLIterator *lliter = LLIterator_Allocate(chain);
  HTKeyValue_t *keyVal;

  // if key is found in bucket (the linked list)
   if (hasKey(lliter, key, &keyVal)) {
    // note the key and value in return paremeters
    keyvalue->key = keyVal->key;
    keyvalue->value = keyVal->value;
    // free iterator
    LLIterator_Free(lliter);
    // found in chain, return true
    return true;
  } else {
    // its not in the chain, return false
    // still free iterator since malloc
    LLIterator_Free(lliter);
    return false;
  }
}

bool HashTable_Remove(HashTable *table,
                      HTKey_t key,
                      HTKeyValue_t *keyvalue) {
  Verify333(table != NULL);

  // STEP 3: implement HashTable_Remove.
  int bucket;
  LinkedList *chain;

  // Calculate which bucket and chain we're inserting into by using helper
  // which turns keyvalues into integer bucket numbers
  bucket = HashKeyToBucketNum(table, key);
  // go into the said bucket and access the linked list there
  chain = table->buckets[bucket];

  // if the buck is empty, nothing to find, so search is false
  if (LinkedList_NumElements(chain) == 0) {
    return false;
  }

  LLIterator *lliter = LLIterator_Allocate(chain);
  HTKeyValue_t *keyVal;

  // if key is found in bucket (the linked list)
   if (hasKey(lliter, key, &keyVal)) {
    // note the key and value in return paremeters
    keyvalue->key = keyVal->key;
    keyvalue->value = keyVal->value;

    // delete the key from the bucket
    LLIterator_Remove(lliter, &LLNoOpFree);
    // free the iterator since we malloc
    LLIterator_Free(lliter);
    // since we allocate space when we inserted, we need to
    // free the space while we are deleting the node
    free(keyVal);
    // drop table size since we removed element
    table->num_elements -= 1;
    return true;
  } else {
    // if the element was not in bucket, we removed nothing
    // so we return false. Still free iterator
    LLIterator_Free(lliter);
    return false;
  }
}


///////////////////////////////////////////////////////////////////////////////
// HTIterator implementation.

HTIterator* HTIterator_Allocate(HashTable *table) {
  HTIterator *iter;
  int         i;

  Verify333(table != NULL);

  iter = (HTIterator *) malloc(sizeof(HTIterator));

  // If the hash table is empty, the iterator is immediately invalid,
  // since it can't point to anything.
  if (table->num_elements == 0) {
    iter->ht = table;
    iter->bucket_it = NULL;
    iter->bucket_idx = INVALID_IDX;
    return iter;
  }

  // Initialize the iterator.  There is at least one element in the
  // table, so find the first element and point the iterator at it.
  iter->ht = table;
  for (i = 0; i < table->num_buckets; i++) {
    if (LinkedList_NumElements(table->buckets[i]) > 0) {
      iter->bucket_idx = i;
      break;
    }
  }
  Verify333(i < table->num_buckets);  // make sure we found it.
  iter->bucket_it = LLIterator_Allocate(table->buckets[iter->bucket_idx]);
  return iter;
}

void HTIterator_Free(HTIterator *iter) {
  Verify333(iter != NULL);
  if (iter->bucket_it != NULL) {
    LLIterator_Free(iter->bucket_it);
    iter->bucket_it = NULL;
  }
  free(iter);
}

bool HTIterator_IsValid(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 4: implement HTIterator_IsValid.
  // when bucket_idx is invalid, iterator is invalid, so just check that
  return (iter->bucket_idx != INVALID_IDX);  // you may need to change this return value
}

bool HTIterator_Next(HTIterator *iter) {
  Verify333(iter != NULL);

  // STEP 5: implement HTIterator_Next.

  // if iterator is invalid, return false
  if(iter->bucket_idx == INVALID_IDX) {
    return false;
  }
  // if the current bucket has next element,
  // return true
  if (LLIterator_Next(iter->bucket_it)) {
    return true;
  }

  // if you're at last idx, set idx to invalid for this iterator
  // and return false
  if (iter->bucket_idx == (iter->ht->num_buckets) - 1) {
    iter->bucket_idx = INVALID_IDX;
    return false;
  }
  // # If there are no more elements in the current bucket,
  // # and we aren't at the end of the table

  // go through the iterator until you find a non empty bucket
  int i = iter->bucket_idx +1;
  while(i < iter->ht->num_buckets ) {
    if (LinkedList_NumElements(iter->ht->buckets[i]) > 0) {
      // set the iterator idx to this new bucket idx that isn't empty
      iter->bucket_idx = i;
      break;
    } i++;
  }
  // if we to the end of the loop above due to i > iter->ht->num_buckers , 
  // it means we went through the entire thing without finding a new non empty bucket,
  // so the iterator is now invalid
  if (i == iter->ht->num_buckets) {
    iter->bucket_idx = INVALID_IDX;
    return false;
  }
  // if we find a non empty bucket, we're cash money
  // we free bucket iterator, and allocate new one based on 
  // new bucket 
  LLIterator_Free(iter->bucket_it);
  iter->bucket_it = LLIterator_Allocate(iter->ht->buckets[iter->bucket_idx]);
  Verify333(iter->bucket_it != NULL);

  return true;  // you may need to change this return value
}

bool HTIterator_Get(HTIterator *iter, HTKeyValue_t *keyvalue) {
  Verify333(iter != NULL);

  // STEP 6: implement HTIterator_Get.

   HTKeyValue_t *valPair;  // variable to store key/value

  // if the iterator is valid
  if (iter->bucket_idx != INVALID_IDX) {
    // get the key/value pair and store
    LLIterator_Get(iter->bucket_it, (void **) &valPair);

    // note the values into return paremeter keyvalue
    keyvalue->key = valPair->key;
    keyvalue->value = valPair->value;
    return true;
  }
  // return false since iter is invalid
  return false;
}

bool HTIterator_Remove(HTIterator *iter, HTKeyValue_t *keyvalue) {
  HTKeyValue_t kv;

  Verify333(iter != NULL);

  // Try to get what the iterator is pointing to.
  if (!HTIterator_Get(iter, &kv)) {
    return false;
  }

  // Advance the iterator.  Thanks to the above call to
  // HTIterator_Get, we know that this iterator is valid (though it
  // may not be valid after this call to HTIterator_Next).
  HTIterator_Next(iter);

  // Lastly, remove the element.  Again, we know this call will succeed
  // due to the successful HTIterator_Get above.
  Verify333(HashTable_Remove(iter->ht, kv.key, keyvalue));
  Verify333(kv.key == keyvalue->key);
  Verify333(kv.value == keyvalue->value);

  return true;
}

static void MaybeResize(HashTable *ht) {
  HashTable *newht;
  HashTable tmp;
  HTIterator *it;

  // Resize if the load factor is > 3.
  if (ht->num_elements < 3 * ht->num_buckets)
    return;

  // This is the resize case.  Allocate a new hashtable,
  // iterate over the old hashtable, do the surgery on
  // the old hashtable record and free up the new hashtable
  // record.
  newht = HashTable_Allocate(ht->num_buckets * 9);

  // Loop through the old ht copying its elements over into the new one.
  for (it = HTIterator_Allocate(ht);
       HTIterator_IsValid(it);
       HTIterator_Next(it)) {
    HTKeyValue_t item, unused;

    Verify333(HTIterator_Get(it, &item));
    HashTable_Insert(newht, item, &unused);
  }

  // Swap the new table onto the old, then free the old table (tricky!).  We
  // use the "no-op free" because we don't actually want to free the elements;
  // they're owned by the new table.
  tmp = *ht;
  *ht = *newht;
  *newht = tmp;

  // Done!  Clean up our iterator and temporary table.
  HTIterator_Free(it);
  HashTable_Free(newht, &HTNoOpFree);
}

// helper function to check if a key is the bucket (linkedlist)
static bool hasKey(LLIterator *lliter, int key, HTKeyValue_t **keyvalue) {
  Verify333(lliter != NULL);
  // check the current node 
  LLIterator_Get(lliter, (void **) keyvalue);
  if ((*keyvalue)->key == key) {
      return true;
    }
  // LLIterator_Next, when called, automatically increments the iterator,
  // so we have to check the first node (as done above), then when the while 
  // statement is done, it automatically increments. If it incremented to a 
  // valid element, it will return true and the while statement runs. If it 
  // increment to a invalid (NULL) element, it will return false, and the 
  // while loop won run
  while (LLIterator_Next(lliter)) {
    // grab node and put it into keyvalue each time
    LLIterator_Get(lliter, (void **) keyvalue);
    // key is found, return true 
    if ((*keyvalue)->key == key) {
      return true;
    }
    // advance to the next node
  }
  // key is not found since the while loop ended without returning true
  return false;
}
