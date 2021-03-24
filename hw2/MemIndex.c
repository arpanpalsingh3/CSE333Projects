/*
 * Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
 * hereby granted to students registered for University of Washington
 * CSE 333 for use solely during Fall Quarter 2020 for purposes of
 * the course.  No other use, copying, distribution, or modification
 * is permitted without prior written consent. Copyrights for
 * third-party components of this work must be honored.  Instructors
 * interested in reusing these course materials should contact the
 * author.
 */

#include "./MemIndex.h"

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "libhw1/CSE333.h"
#include "libhw1/HashTable.h"
#include "libhw1/LinkedList.h"


///////////////////////////////////////////////////////////////////////////////
// Internal-only helpers
//
// These functions use the MI_ prefix instead of the MemIndex_ prefix,
// to indicate that they should not be considered part of the MemIndex's
// public API.

// Comparator usable by LinkedList_Sort(), which implements an increasing
// order by rank over SearchResult's.  If the caller is interested in a
// decreasing sort order, they should invert the return value.
static int MI_SearchResultComparator(LLPayload_t e1, LLPayload_t e2) {
  SearchResult *sr1 = (SearchResult *) e1;
  SearchResult *sr2 = (SearchResult *) e2;

  if (sr1->rank > sr2->rank) {
    return 1;
  } else if (sr1->rank < sr2->rank) {
    return -1;
  } else {
    return 0;
  }
}

// Deallocator usable by LinkedList_Free(), which frees a list of
// of DocPositionOffset_t.  Since these offsets are stored inline (ie, not
// malloc'ed), there is nothing to do in this function.
static void MI_NoOpFree(LLPayload_t ptr) { }

// Deallocator usable by HashTable_Free(), which frees a LinkedList of
// DocPositionOffset_t (ie, our posting list).  We use these LinkedLists
// in our WordPostings.
static void MI_PostingsFree(HTValue_t ptr) {
  LinkedList *list = (LinkedList *) ptr;
  LinkedList_Free(list, &MI_NoOpFree);
}

// Deallocator used by HashTable_Free(), which frees a WordPostings.  A
// MemIndex consists of a HashTable of WordPostings (ie, this is the top-level
// structure).
static void MI_ValueFree(HTValue_t ptr) {
  WordPostings *wp = (WordPostings *) ptr;

  free(wp->word);
  HashTable_Free(wp->postings, &MI_PostingsFree);

  free(wp);
}

///////////////////////////////////////////////////////////////////////////////
// MemIndex implementation

MemIndex* MemIndex_Allocate(void) {
  // Happily, HashTables dynamically resize themselves, so we can start by
  // allocating a small hashtable.
  HashTable *mi = HashTable_Allocate(16);
  Verify333(mi != NULL);
  return mi;
}

void MemIndex_Free(MemIndex *index) {
  HashTable_Free(index, &MI_ValueFree);
}

int MemIndex_NumWords(MemIndex *index) {
  return HashTable_NumElements(index);
}

void MemIndex_AddPostingList(MemIndex *index, char *word, DocID_t docid,
                             LinkedList *postings) {
  HTKey_t key = FNVHash64((unsigned char *) word, strlen(word));
  HTKeyValue_t mi_kv, postings_kv, unused;
  WordPostings *wp;

  // STEP 1.
  // Remove this early return.  We added this in here so that your unittests
  // would pass even if you haven't finished your MemIndex implementation.
  // return;


  // First, we have to see if the passed-in word already exists in
  // the inverted index.
  if (!HashTable_Find(index, key, &mi_kv)) {
    // STEP 2.
    // No, this is the first time the inverted index has seen this word.  We
    // need to prepare and insert a new WordPostings structure.  After
    // malloc'ing it, we need to:
    //   (1) find existing memory or allocate new memory for the WordPostings'
    //       word field (hint: remember that this function takes ownership
    //       of the passed-in word).
    //   (2) allocate a new hashtable for the WordPostings' docID->postings
    //       mapping.
    //   (3) insert the the new WordPostings into the inverted index (ie, into
    //       the "index" table).

    // new wp struct
    wp = (WordPostings*) malloc(sizeof(WordPostings));
    Verify333(wp != NULL);
    // step 1: find existing memory...
    wp->word = word;
    // step 2: allocate a new hashtable...
    wp->postings = HashTable_Allocate(16);
    Verify333(wp->postings != NULL);
    // step 3: insert the new wordpostings...
    mi_kv.key = key;
    mi_kv.value = (void *) wp;
    HashTable_Insert(index, mi_kv, &unused); 
    // dont care about the returned value from insert 
    // since there is no return in this method


  } else {
    // Yes, this word already exists in the inverted index.  There's no need
    // to insert it again.

    // Instead of allocating a new WordPostings, we'll use the one that's
    // already in the inverted index.
    wp = (WordPostings *) mi_kv.value;

    // Ensure we don't have hash collisions (two different words that hash to
    // the same key, which is very unlikely).
    Verify333(strcmp(wp->word, word) == 0);

    // Now we can free the word (since the caller gave us ownership of it).
    free(word);
  }

  // At this point, we have a WordPostings struct which represents the posting
  // list for this word.  Add this new document's postings to it.

  // Verify that this document is not already in the posting list.
  Verify333(!HashTable_Find(wp->postings, docid, &unused));

  // STEP 3.
  // Insert a new entry into the wp->postings hash table.
  // The entry's key is this docID and the entry's value
  // is the "postings" (ie, word positions list) we were passed
  // as an argument.
  postings_kv.value = postings;
  postings_kv.key = docid;
  HashTable_Insert(wp->postings, postings_kv, &unused);
  // again don't care about value returned from insert


}

LinkedList* MemIndex_Search(MemIndex *index, char *query[], int querylen) {
  LinkedList* retlist;
  HTKeyValue_t kv;
  WordPostings *wp;
  HTKey_t key;
  int i;

  // If the user provided us with an empty search query, return NULL
  // to indicate failure.
  if (querylen == 0) {
    return NULL;
  }

  // STEP 4.
  // The most interesting part of Part C starts here...!
  //
  // Look up the first query word (ie, query[0]) in the inverted index.  For
  // each document that matches, allocate and initialize a SearchResult
  // structure (the initial computed rank is the number of times the word
  // appears in that document).  Finally, append the SearchResult onto retline.

  // We hash query[0], then search the MemIndex for this key
  key = FNVHash64((unsigned char *) query[0], strlen(query[0]));
  // if the search is false, then return NULL to 
  // indicate failure 
  if(!HashTable_Find(index,key, &kv)) {
    return NULL;
  }
  // If it is in the index, we now allocate based on this key
  retlist = LinkedList_Allocate();
  wp = kv.value;
  HTIterator* hIter = HTIterator_Allocate(wp->postings);
  Verify333(hIter != NULL);
  
  bool hasNext = true;
  // Create searchresults for each match
  while(hasNext) {
    // grab the current element in the iterator 
    HTIterator_Get(hIter,&kv);
    // allocate SR
    SearchResult *sr = (SearchResult *) malloc(sizeof(SearchResult));
    Verify333(sr != NULL);

    sr->docid = kv.key; // the id is the key
    // rank is number of occurences, which is just the number of elements 
    sr->rank = LinkedList_NumElements(kv.value); 

    // add searchResult of the document into retlist
    LinkedList_Append(retlist, sr);
    // move the iterator forward
    hasNext = HTIterator_Next(hIter);
  }
  HTIterator_Free(hIter);


  // Great; we have our search results for the first query
  // word.  If there is only one query word, we're done!
  // Sort the result list and return it to the caller.
  if (querylen == 1) {
    LinkedList_Sort(retlist, false, &MI_SearchResultComparator);
    return retlist;
  }

  // OK, there are additional query words.  Handle them one
  // at a time.
  for (i = 1; i < querylen; i++) {
    LLIterator *llit;
    int j, numdocs;

    // STEP 5.
    // Look up the next query word (query[i]) in the inverted index.
    // If there are no matches, it means the overall query
    // should return no documents, so free retlist and return NULL.
  
    key = FNVHash64((unsigned char *) query[i], strlen(query[i]));

    if(!HashTable_Find(index,key, &kv)) {
      LinkedList_Free(retlist, &free);
      return NULL;
    }

    // STEP 6.
    // There are matches.  We're going to iterate through
    // the docIDs in our current search result list, testing each 
    // to see whether it is also in the set of matches for
    // the query[i].
    //
    // If it is, we leave it in the search
    // result list and we update its rank by adding in the
    // number of matches for the current word.
    //
    // If it isn't, we delete that docID from the search result list.
    llit = LLIterator_Allocate(retlist);
    Verify333(llit != NULL);
    numdocs = LinkedList_NumElements(retlist);
    wp = kv.value;
    for (j = 0; j < numdocs; j++) {
      SearchResult *sr;
      LLIterator_Get(llit, (void **) &sr);
      // check if the document currently in the retlist
      // is also in the qp

      if (!HashTable_Find(wp->postings, sr->docid, &kv)) {
        // delete the document which doesn't contain the word 
        LLIterator_Remove(llit, &free);
      } else {
        // update the rank of the document's search result
        // by number of appearance of the word in the document
        sr->rank +=  LinkedList_NumElements( kv.value);
        LLIterator_Next(llit);
      }
    }
    LLIterator_Free(llit);

    // We've finished processing this current query word.  If there are no
    // documents left in our result list, free retlist and return NULL.
    if (LinkedList_NumElements(retlist) == 0) {
      LinkedList_Free(retlist, (LLPayloadFreeFnPtr)free);
      return NULL;
    }
  }

  // Sort the result list by rank and return it to the caller.
  LinkedList_Sort(retlist, false, &MI_SearchResultComparator);
  return retlist;
}
