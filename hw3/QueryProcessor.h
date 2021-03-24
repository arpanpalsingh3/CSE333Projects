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

#ifndef HW3_QUERYPROCESSOR_H_
#define HW3_QUERYPROCESSOR_H_

#include <cstdio>
#include <list>
#include <vector>
#include <string>

#include "./DocIDTableReader.h"
#include "./DocTableReader.h"
#include "./FileIndexReader.h"
#include "./IndexTableReader.h"
#include "./Utils.h"

using std::list;
using std::vector;
using std::string;

namespace hw3 {

// A QueryProcessor is a class that is given a set of names of index
// files, and uses the various FileIndexReader and HashTableReader
// classes to process queries against the indices.
class QueryProcessor {
 public:
  // Construct a QueryProcessor.  Arguments:
  //
  // - indexlist: a std::list<std::string> containing a list of index
  //   filenames that the QueryProcessor should use.
  //
  // - validate: a bool indicating whether or not to validate the
  //   checksums in the index files.  Defaults to true.
  explicit QueryProcessor(const std::list<std::string>& indexlist,
                          bool validate = true);

  // The destructor.
  ~QueryProcessor();

  // This structure defines a single query result.  As with HW2,
  // the rank of a query result is the sum of the number of occurrences
  // of query words within the document.
  class QueryResult {
   public:
    bool operator<(const QueryResult& rhs) const { return rank > rhs.rank; }
    std::string documentName;  // The name of a matching document.
    int  rank;                 // The rank of the matching document.
  };

  // This method processes a query against the indices and returns a
  // vector of QueryResults, sorted in descending order of rank.  If no
  // documents match the query, then a valid but empty vector will be
  // returned.
  std::vector<QueryResult> ProcessQuery(const std::vector<std::string>& query);

 protected:
  // The list of index files we process.
  std::list<std::string> indexlist_;

  // The arrays of pointers to DocTableReader and IndexTableReader
  // objects.
  int                 arraylen_;
  DocTableReader    **dtr_array_;
  IndexTableReader  **itr_array_;

 private:
  DISALLOW_COPY_AND_ASSIGN(QueryProcessor);
};

}  // namespace hw3

#endif  // HW3_QUERYPROCESSOR_H_
