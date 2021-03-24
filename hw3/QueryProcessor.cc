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

#include "./QueryProcessor.h"

#include <iostream>
#include <algorithm>

extern "C" {
  #include "./libhw1/CSE333.h"
}

using std::list;
using std::sort;
using std::string;
using std::vector;

namespace hw3 {

QueryProcessor::QueryProcessor(const list<string> &indexlist, bool validate) {
  // Stash away a copy of the index list.
  indexlist_ = indexlist;
  arraylen_ = indexlist_.size();
  Verify333(arraylen_ > 0);

  // Create the arrays of DocTableReader*'s. and IndexTableReader*'s.
  dtr_array_ = new DocTableReader *[arraylen_];
  itr_array_ = new IndexTableReader *[arraylen_];

  // Populate the arrays with heap-allocated DocTableReader and
  // IndexTableReader object instances.
  list<string>::const_iterator idx_iterator = indexlist_.begin();
  for (int i = 0; i < arraylen_; i++) {
    FileIndexReader fir(*idx_iterator, validate);
    dtr_array_[i] = fir.NewDocTableReader();
    itr_array_[i] = fir.NewIndexTableReader();
    idx_iterator++;
  }
}

QueryProcessor::~QueryProcessor() {
  // Delete the heap-allocated DocTableReader and IndexTableReader
  // object instances.
  Verify333(dtr_array_ != nullptr);
  Verify333(itr_array_ != nullptr);
  for (int i = 0; i < arraylen_; i++) {
    delete dtr_array_[i];
    delete itr_array_[i];
  }

  // Delete the arrays of DocTableReader*'s and IndexTableReader*'s.
  delete[] dtr_array_;
  delete[] itr_array_;
  dtr_array_ = nullptr;
  itr_array_ = nullptr;
}

// This structure is used to store a index-file-specific query result.
typedef struct {
  DocID_t docid;  // The document ID within the index file.
  int rank;       // The rank of the result so far.
} IdxQueryResult;
/*
  The purpose of this method is to allow me to combine two lists which contain 
  DocIds along with the number of appearances of the specified word in that
  specific doc.
*/
static list<DocIDElementHeader> ListCombine( const list<DocIDElementHeader> &l1, const list<DocIDElementHeader> &l2) {
  list<DocIDElementHeader>::const_iterator it1;
  list<DocIDElementHeader>::const_iterator it2;
  list<DocIDElementHeader> result;
  // Go through all the elements in l1, and compare each with 
  // all the elements from l2.
  for (it1 = l1.begin(); it1 != l1.end(); it1++) {
    for (it2 = l2.begin(); it2 != l2.end(); it2++) {
      // if found in both, append docID and sum of the ranks to result
      // the new addition represents the sum
      if (it1->docID == it2->docID) {
        uint32_t total_rank = it1->numPositions + it2->numPositions;
        result.push_back(DocIDElementHeader(it1->docID, total_rank));
        break;
      }
    }
  }
  return result;
}

vector<QueryProcessor::QueryResult>
QueryProcessor::ProcessQuery(const vector<string> &query) {
  Verify333(query.size() > 0);

  // STEP 1.
  // (the only step in this file)
  vector<QueryProcessor::QueryResult> finalresult;

  // Run for loop for each index in indexlist 
  for(int i = 0; i < arraylen_; i++) {
    // Initializing necessary arguments 

    // When i = 0, we grab the first set of doctable/indextable readers
    // Meaning, with each iteration, we are checking each individual 
    // index, then moving on.
    DocTableReader *docTr = dtr_array_[i];
    IndexTableReader *indexTr = itr_array_[i];
    list<DocIDElementHeader> result; 
    DocIDTableReader *DocIdTr;

    // When query is length only 1, we only need to work with the results
    // list acquired from query[0], then move onto next index
    if(query.size() == 1) {
      // Find the first word and return DocIDTr associated with it
      DocIdTr = indexTr->LookupWord(query[0]);
      // If the word was not found
      if(DocIdTr == nullptr) {
        continue; // Move on in the loop
      }
      // Extract the DocIDlist into a list
      result = DocIdTr->GetDocIDList();
      // Delete after extraction, no longer needed
      delete DocIdTr;

      string filename;
      list<DocIDElementHeader>::iterator it;
      // iterate through the list 
      for(it = result.begin(); it != result.end(); it++) {
        // Get the file name from the docTr 
        Verify333(docTr->LookupDocID(it->docID, &filename));
        // Create a QueryResult using the file name
        QueryResult qr;
        qr.documentName = filename;
        qr.rank =  it->numPositions; // the number of times this query appeared in this index
        // Add to the result
        finalresult.push_back(qr);
      }
      // move on to next index
      continue;
    }

    // If the qeuery size > 1, we have to check this index for each word before moving
    // on to the next index, hence a for loop going through each word.
    for(uint32_t j = 1; j < query.size(); j++) {
      if(j == 1) { 
        // Find the first word and return DocIDTr associated with it
        DocIdTr = indexTr->LookupWord(query[0]);
        // If the word was not found
        if(DocIdTr == nullptr) {
          continue; // Move on in the loop
        }
        // Extract the DocIDlist into a list
        result = DocIdTr->GetDocIDList();
        // Delete after extraction, no longer needed
        delete DocIdTr;
      }

      list<DocIDElementHeader> secondList; 
      // Grab the next Query
      DocIdTr = indexTr->LookupWord(query[j]); // look up the second word in the query
      // Make sure it's not null
      if(DocIdTr == nullptr) {
        result.clear();
        break;
      }
      // Get the DocID list
      secondList = DocIdTr->GetDocIDList();

      delete DocIdTr;
      // Now, compare the two lists
      result = ListCombine(result, secondList);
      // results is the combination of the two lists 
      if(result.size() == 0) {
        break;
      }
    }
    // After all the queries, we should have a result list
    if(result.size() != 0 ) {
      string filename;
      list<DocIDElementHeader>::iterator it;
      // Iterate through the result list
      for(it = result.begin(); it != result.end(); it++) {
        // Note the file name for this specific DocID
        Verify333(docTr->LookupDocID(it->docID, &filename));
        // Create QueryResult with the file name and number of occurences for this query 
        QueryResult qr;
        qr.documentName = filename;
        qr.rank =  it->numPositions; // Number of occurances of this query in this specificed query
        finalresult.push_back(qr);
      }
    }
  }

  // Sort the final results.
  sort(finalresult.begin(), finalresult.end());
  return finalresult;
}


}  // namespace hw3
