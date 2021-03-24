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

// Feature test macro for strtok_r (c.f., Linux Programming Interface p. 63)
#define _XOPEN_SOURCE 600

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#include "libhw1/CSE333.h"
#include "./CrawlFileTree.h"
#include "./DocTable.h"
#include "./MemIndex.h"

//////////////////////////////////////////////////////////////////////////////
// Helper function declarations, constants, etc
static void Usage(void);
static void PrintOutput(DocTable *dt, LinkedList *retlist);
static int GetNextLine(FILE *f, char **retstr);


//////////////////////////////////////////////////////////////////////////////
// Main
int main(int argc, char **argv) {
  if (argc != 2) {
    Usage();
  }

  // Implement searchshell!  We're giving you very few hints
  // on how to do it, so you'll need to figure out an appropriate
  // decomposition into functions as well as implementing the
  // functions.  There are several major tasks you need to build:
  //
  //  - Crawl from a directory provided by argv[1] to produce and index
  //  - Prompt the user for a query and read the query from stdin, in a loop
  //  - Split a query into words (check out strtok_r)
  //  - Process a query against the index and print out the results
  //
  // When searchshell detects end-of-file on stdin (cntrl-D from the
  // keyboard), searchshell should free all dynamically allocated
  // memory and any other allocated resources and then exit.

  MemIndex* index;
  DocTable* table;
  LinkedList* retlist;
  
  // Starting message 
  printf("Indexing '%s'\n", argv[1]);
  // step 1: Crawl from a directory provided....
  if(!CrawlFileTree(argv[1], &table, &index)) {
    fprintf(stderr, "Path '%s' is not indexable\n", argv[1]);  // error
    Usage();
  }
  // from this point on, we are good to go since we got a proper directory 

  char* query[32];
  int queryLen;
  // continious loop until client quits 
  while(1) {
    // we prompt user using GetNextLine and simultaniously acquire the 
    // string version of thier input into query and length of query into queryLen
    queryLen = GetNextLine(stdin,query);
    // MemIndex search using the provided information, which
    // outputs a linked list with all the data we need
    retlist = MemIndex_Search(index,query,queryLen);
    // so long as the output is SOMETHING...
    if (retlist != NULL) {
      // print our results from the search 
      PrintOutput(table, retlist);
    }
    queryLen = 0;  // reset
  }
  // free allocations 
  MemIndex_Free(index);
  DocTable_Free(table);

  return EXIT_SUCCESS;

}


//////////////////////////////////////////////////////////////////////////////
// Helper function definitions

static void Usage(void) {
  fprintf(stderr, "Usage: ./searchshell <docroot>\n");
  fprintf(stderr,
          "where <docroot> is an absolute or relative " \
          "path to a directory to build an index under.\n");
  exit(EXIT_FAILURE);
}
/*
  Takes in an input stream f and double * retstr. It prompts 
  the user to write in the query they want to search in the specified 
  directory, then reads and splits the query into specific words using 
  strtok_r. Those words are inserted into retstr one by one. While each "word"
  is added into retstr, we keep track of the number of words we are adding 
  in qlen.
  Argument: 
    *f: The input stream we are reading from 
    **retstr: The output string which contains all the "words" we parsed
              from the input stream
  Returns: qlen (the number of words we parsed from the user input)
*/
static int GetNextLine(FILE *f, char **retstr) {
  int qlen = 0; // our tracker for the length of retstr
  int bufsize = 32;
  char *streamRead = (char *) malloc(bufsize);
  char *save;

  // Prompt user for input
  printf("enter query:\n");

  // read from stream and put into buf 
  fgets(streamRead, bufsize-1, f);

  // swap newline character for end character
  for (int i = 0; i < bufsize-1; i++) {
    if (streamRead[i] == '\n') {
      streamRead[i] = '\0';
      break;
    }
  }
  // get the first element of our split up string 
  // using strtok_r
  retstr[qlen] = strtok_r(streamRead, " ", &save);

  // if NULL input, we got a problem
  if (retstr[qlen] == NULL) {
    printf("Not a valid input (ctrl-C to quit)\n");
  } else {
    // if the first element is good, we go through the rest
    qlen++; // increment 
    while (1) {
      // grab the next query and put into now incremented index
      retstr[qlen] = strtok_r(NULL, " ", &save);
      // check if now this is null or not
      if (retstr[qlen] == NULL)  
        break; // if null, break the while loop
      // if valid, increment qlen tracker
      qlen++;
    }
  }
  return qlen;
}
/*
  Print output takes in a DocTable and a Linkedlist
  and using those 2, outputs all elements fromt he Linkedlist
  in the desired output format
  Arguments: 
    *dt: The doctable we are getting our docnames from
    *retlist: which contains all the results from our memindex search
  Returns: Nothing, just prints out stuff
*/
static void PrintOutput(DocTable *dt, LinkedList *retlist) {
  SearchResult *sr;
  int totalElements = LinkedList_NumElements(retlist);
  LLIterator *llit = LLIterator_Allocate(retlist);
  // for every element in the linkedList retlist 
  for (int i = 0; i < totalElements; i++) {
    // grab the element from the linkedlist, and put it into searchresult 
    LLIterator_Get(llit, (LLPayload_t *) &sr);
    // print by reading through the search result 
    printf("  %s (%u)\n", DocTable_GetDocName(dt, sr->docid), sr->rank);
    // remove from the list
    LLIterator_Remove(llit, &free);
    // keep repeating until all elements are printed
  } 
  // free allocations 
  LLIterator_Free(llit);
  LinkedList_Free(retlist, &free);

  return;
}

