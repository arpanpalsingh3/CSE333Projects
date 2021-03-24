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

#include <stdint.h>
#include <boost/algorithm/string.hpp>
#include <boost/lexical_cast.hpp>
#include <map>
#include <string>
#include <vector>
#include <iostream>

#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpConnection.h"

using std::map;
using std::string;
using std::vector;

namespace hw4 {

static const char *kHeaderEnd = "\r\n\r\n";
static const int kHeaderEndLen = 4;

bool HttpConnection::GetNextRequest(HttpRequest *request) {
  // Use "WrappedRead" to read data into the buffer_
  // instance variable.  Keep reading data until either the
  // connection drops or you see a "\r\n\r\n" that demarcates
  // the end of the request header. Be sure to try and read in
  // a large amount of bytes each time you call WrappedRead.
  //
  // Once you've seen the request header, use ParseRequest()
  // to parse the header into the *request argument.
  //
  // Very tricky part:  clients can send back-to-back requests
  // on the same socket.  So, you need to preserve everything
  // after the "\r\n\r\n" in buffer_ for the next time the
  // caller invokes GetNextRequest()!

  // STEP 1:
  
  // First we check the header_ to see if there is already 
  // a request in it (repetitive call to same socket)
  size_t endCheck = buffer_.find(kHeaderEnd);
  // If this is a fresh request, we read from FD into buffer_
  if(endCheck == std::string::npos) {
    int read; 
    // The buffer needed for a wrapped read to read into
    unsigned char buf[1024];
    // Read until something breaks 
    while(1) {
      read = WrappedRead(fd_,buf,1024);
      if(read == -1) {
        // Fatal Error
        return false;
      } 
      else if (read == 0) {
        // EOF
        break;
      } 
      else {  
        // Append what we read into buffer_
        buffer_ += std::string(reinterpret_cast<char*>(buf), read);
        // Now check if we have a kHeaderEnd (meaning a full request)
        endCheck = buffer_.find(kHeaderEnd);
        // If we do have a request, end the loop, and move to parsing
        if(endCheck != std::string::npos) {
          // We read the end of the request header, break while loop
          break;
        }
      }
    }
  }
  
  // If we got to EOF, and we never got a kHeaderEnd, that means
  // something was wrong, return false
  if(endCheck == std::string::npos) {
    return false;
  } 

  // Parse the first request, which is from start of buffer
  // to the location of kHeaderEnd.
  *request = ParseRequest(buffer_.substr(0,endCheck + kHeaderEndLen));

  // Remove what we just parsed from the buffer_, so that if there are more
  // requests in the buffer, they can be parsed 
  buffer_ = buffer_.substr(endCheck + kHeaderEndLen);
  
  return true;  // Finished
}

bool HttpConnection::WriteResponse(const HttpResponse &response) {
  string str = response.GenerateResponseString();
  int res = WrappedWrite(fd_,
                         (unsigned char *) str.c_str(),
                         str.length());
  if (res != static_cast<int>(str.length()))
    return false;
  return true;
}

HttpRequest HttpConnection::ParseRequest(const string &request) {
  HttpRequest req("/");  // by default, get "/".

  // Split the request into lines.  Extract the URI from the first line
  // and store it in req.URI.  For each additional line beyond the
  // first, extract out the header name and value and store them in
  // req.headers_ (i.e., HttpRequest::AddHeader).  You should look
  // at HttpRequest.h for details about the HTTP header format that
  // you need to parse.
  //
  // You'll probably want to look up boost functions for (a) splitting
  // a string into lines on a "\r\n" delimiter, (b) trimming
  // whitespace from the end of a string, and (c) converting a string
  // to lowercase.
  //
  // Note that you may assume the request you are parsing is correctly
  // formatted. If for some reason you encounter a header that is
  // malformed, you may skip that line.

  // STEP 2:
  // Acquire the request 
  std::string reqStr = request;

  // Split into lines 
  std::vector<std::string> lines;
  boost::split(lines, reqStr, boost::is_any_of("\r\n"), boost::token_compress_on); 

  // Extract the first line, and split by " " 
  std::vector<std::string> line;
  boost::split(line, lines[0], boost::is_any_of(" "), boost::token_compress_on);
  // Extract URI from line 1, and set req uri to it
  req.set_uri(line[1]);

  // For each line after the first, extract header name and header val
  // Last line is unecessary 
  for(uint32_t i = 1; i < lines.size()-1; i++) {
    // Acquire the individual parts of the line
    boost::split(line, lines[i], boost::is_any_of(":"), boost::token_compress_on);  
    // Lowercase the header name
    boost::to_lower(line[0]);
    // Trim whitespace from header value since we split at ":" and there is 
    // white space after that 
    boost::trim(line[1]);
    // Add the mapping
    req.AddHeader(line[0],line[1]);
  } 

  return req;
}

}  // namespace hw4
