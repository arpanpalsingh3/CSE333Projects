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

#include <boost/algorithm/string.hpp>
#include <iostream>
#include <map>
#include <memory>
#include <vector>
#include <string>
#include <sstream>

#include "./FileReader.h"
#include "./HttpConnection.h"
#include "./HttpRequest.h"
#include "./HttpUtils.h"
#include "./HttpServer.h"
#include "./libhw3/QueryProcessor.h"

using std::cerr;
using std::cout;
using std::endl;
using std::list;
using std::map;
using std::string;
using std::stringstream;
using std::unique_ptr;

namespace hw4 {
///////////////////////////////////////////////////////////////////////////////
// Constants, internal helper functions
///////////////////////////////////////////////////////////////////////////////
static const char *kThreegleStr =
  "<html><head><title>333gle</title></head>\n"
  "<body>\n"
  "<center style=\"font-size:500%;\">\n"
  "<span style=\"position:relative;bottom:-0.33em;color:orange;\">3</span>"
    "<span style=\"color:red;\">3</span>"
    "<span style=\"color:gold;\">3</span>"
    "<span style=\"color:blue;\">g</span>"
    "<span style=\"color:green;\">l</span>"
    "<span style=\"color:red;\">e</span>\n"
  "</center>\n"
  "<p>\n"
  "<div style=\"height:20px;\"></div>\n"
  "<center>\n"
  "<form action=\"/query\" method=\"get\">\n"
  "<input type=\"text\" size=30 name=\"terms\" />\n"
  "<input type=\"submit\" value=\"Search\" />\n"
  "</form>\n"
  "</center><p>\n";

// static
const int HttpServer::kNumThreads = 100;

// This is the function that threads are dispatched into
// in order to process new client connections.
static void HttpServer_ThrFn(ThreadPool::Task *t);

// Given a request, produce a response.
static HttpResponse ProcessRequest(const HttpRequest &req,
                            const string &basedir,
                            const list<string> *indices);

// Process a file request.
static HttpResponse ProcessFileRequest(const string &uri,
                                const string &basedir);

// Process a query request.
static HttpResponse ProcessQueryRequest(const string &uri,
                                 const list<string> *indices);


///////////////////////////////////////////////////////////////////////////////
// HttpServer
///////////////////////////////////////////////////////////////////////////////
bool HttpServer::Run(void) {
  // Create the server listening socket.
  int listen_fd;
  cout << "  creating and binding the listening socket..." << endl;
  if (!ss_.BindAndListen(AF_INET6, &listen_fd)) {
    cerr << endl << "Couldn't bind to the listening socket." << endl;
    return false;
  }

  // Spin, accepting connections and dispatching them.  Use a
  // threadpool to dispatch connections into their own thread.
  cout << "  accepting connections..." << endl << endl;
  ThreadPool tp(kNumThreads);
  while (1) {
    HttpServerTask *hst = new HttpServerTask(HttpServer_ThrFn);
    hst->basedir = staticfileDirpath_;
    hst->indices = &indices_;
    if (!ss_.Accept(&hst->client_fd,
                    &hst->caddr,
                    &hst->cport,
                    &hst->cdns,
                    &hst->saddr,
                    &hst->sdns)) {
      // The accept failed for some reason, so quit out of the server.
      // (Will happen when kill command is used to shut down the server.)
      break;
    }
    // The accept succeeded; dispatch it.
    tp.Dispatch(hst);
  }
  return true;
}

static void HttpServer_ThrFn(ThreadPool::Task *t) {
  // Cast back our HttpServerTask structure with all of our new
  // client's information in it.
  unique_ptr<HttpServerTask> hst(static_cast<HttpServerTask *>(t));
  cout << "  client " << hst->cdns << ":" << hst->cport << " "
       << "(IP address " << hst->caddr << ")" << " connected." << endl;

  // Read in the next request, process it, write the response.

  // Use the HttpConnection class to read and process the next
  // request from our current client, then write out our response.  If
  // the client sends a "Connection: close\r\n" header, then shut down
  // the connection -- we're done.
  //
  // Hint: the client can make multiple requests on our single connection,
  // so we should keep the connection open between requests rather than
  // creating/destroying the same connection repeatedly.

  // STEP 1:
  // Create the connection 
  HttpConnection hc(hst->client_fd);
  bool done = false;
  while (!done) { // Keeps our connection open until done 
    HttpRequest req;
    // Read the request, and if false, it means no 
    // request left, so close the connection 
    if (!hc.GetNextRequest(&req)) {
      close(hst->client_fd);
      done = true; // End loop
    }
    // Process the request 
    HttpResponse res = ProcessRequest(req, hst->basedir, hst->indices);

    // Write the response
    if (!hc.WriteResponse(res)) {
      close(hst->client_fd);
      done = true;
    }

    // Close the connection when client sends close 
    if (req.GetHeaderValue("connection") == "close") {
      close(hst->client_fd);
      done = true;
    }
  }
}

static HttpResponse ProcessRequest(const HttpRequest &req,
                            const string &basedir,
                            const list<string> *indices) {
  // Is the user asking for a static file?
  if (req.uri().substr(0, 8) == "/static/") {
    return ProcessFileRequest(req.uri(), basedir);
  }

  // The user must be asking for a query.
  return ProcessQueryRequest(req.uri(), indices);
}

static HttpResponse ProcessFileRequest(const string &uri,
                                const string &basedir) {
  // The response we'll build up.
  HttpResponse ret;

  // Steps to follow:
  //  - use the URLParser class to figure out what filename
  //    the user is asking for.
  //
  //  - use the FileReader class to read the file into memory
  //
  //  - copy the file content into the ret.body
  //
  //  - depending on the file name suffix, set the response
  //    Content-type header as appropriate, e.g.,:
  //      --> for ".html" or ".htm", set to "text/html"
  //      --> for ".jpeg" or ".jpg", set to "image/jpeg"
  //      --> for ".png", set to "image/png"
  //      etc.
  //
  // be sure to set the response code, protocol, and message
  // in the HttpResponse as well.
  string fname = "";

  // STEP 2:
  // Parse and get file name
  URLParser parser;
  parser.Parse(uri);
  fname += parser.path();
  // Since this is a file request, we want to get rid of the "/static/"
  fname = fname.replace(0, 8, "");
  FileReader fr(basedir, fname);
  std::string read;
  if(fr.ReadFile(&read)) {
    // get the file name suffix
    size_t dot_pos = fname.rfind(".");
    std::string suffix = fname.substr(dot_pos, fname.length() - 1);
      
    // set content type based on suffix
    if (suffix == ".html" || suffix == ".htm")
      ret.set_content_type("text/html");
    else if (suffix == ".jpg" || suffix == ".jpeg")
      ret.set_content_type("image/jpeg");
    else if (suffix == ".png")
      ret.set_content_type("image/png");
    else if (suffix == ".js")
      ret.set_content_type("text/javascript");
    else if (suffix == ".css")
      ret.set_content_type("text/css");        
    else if (suffix == ".csv")
      ret.set_content_type("text/csv");
    else if (suffix == ".xml")
      ret.set_content_type("text/xml");
    else if (suffix == ".gif")
      ret.set_content_type("image/gif");
    else if (suffix == ".tiff")
      ret.set_content_type("image/tiff");
    else 
      ret.set_content_type("text/plain");  
    
    // set the response protocol, response code, and message
    ret.set_protocol("HTTP/1.1");
    ret.set_response_code(200);
    ret.set_message("ITS WORKING!!");
    ret.AppendToBody(read);
    return ret;
  }

  // If you couldn't find the file, return an HTTP 404 error.
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(404);
  ret.set_message("Not Found");
  ret.AppendToBody("<html><body>Couldn't find file \""
                   + EscapeHtml(fname)
                   + "\"</body></html>");
  return ret;
}

static HttpResponse ProcessQueryRequest(const string &uri,
                                 const list<string> *indices) {
  // The response we're building up.
  HttpResponse ret;

  // Your job here is to figure out how to present the user with
  // the same query interface as our solution_binaries/http333d server.
  // A couple of notes:
  //
  //  - no matter what, you need to present the 333gle logo and the
  //    search box/button
  //
  //  - if the user had previously typed in a search query, you also
  //    need to display the search results.
  //
  //  - you'll want to use the URLParser to parse the uri and extract
  //    search terms from a typed-in search query.  convert them
  //    to lower case.
  //
  //  - you'll want to create and use a hw3::QueryProcessor to process
  //    the query against the search indices
  //
  //  - in your generated search results, see if you can figure out
  //    how to hyperlink results to the file contents, like we did
  //    in our solution_binaries/http333d.

  // STEP 3:
  /*
    The 333gle logo and search box 
  */
  ret.AppendToBody(kThreegleStr);

  /*
    The following is for processing a search inside the search box 
  */

  // Parse the uri and get the query
  URLParser parser;
  parser.Parse(uri);
  std::string query = parser.args()["terms"];
  // Lower and trim 
  boost::trim(query);
  boost::to_lower(query);

  // If we got a query, get the query results 
  if (uri.find("query?terms=") != std::string::npos) {
    // Split the query into words, each word split at " "
    std::vector<std::string> queryWords;
    boost::split(queryWords, query, boost::is_any_of(" "), boost::token_compress_on);

    // Create the QueryProcessor
    hw3::QueryProcessor qp(*indices, false);

    // Create the QueryResult from the QueryProcessor by using ProcessQuery 
    std::vector<hw3::QueryProcessor::QueryResult> qr = qp.ProcessQuery(queryWords);
    
    if(qr.size() == 0) {
      // Means we have no results, hence print no results 
      ret.AppendToBody("<p><br>\r\n No results found for <b>" + EscapeHtml(query) + "</b>\r\n<p>\r\n\r\n" );
    } else {
      // Print the total number of matches 
      std::stringstream ss;
      ss << qr.size();
      ret.AppendToBody("<p><br>\r\n" + ss.str());
      ss.str("");
      // Check for singular or plural ;) 
      (qr.size() == 1) ? ret.AppendToBody(" result") : ret.AppendToBody(" results");
      ret.AppendToBody(" found for <b>" + EscapeHtml(query) + "</b>\r\n<p>\r\n\r\n");
    

      // Print each match with hyperlink. Loop through whole query results 
      ret.AppendToBody("<ul>\r\n");
      for(uint32_t i = 0; i < qr.size(); i++) {
        ret.AppendToBody(" <li> <a href=\"");
        // If the document name is NOT a url, then use static 
        if(qr[i].documentName.substr(0,7) != "http://") {
          ret.AppendToBody("/static/");
        }
        ret.AppendToBody(qr[i].documentName + "\">" +EscapeHtml(qr[i].documentName) + "</a> [");
        ss << qr[i].rank;
        ret.AppendToBody(ss.str());
        ss.str("");
        ret.AppendToBody("]<br>\r\n");
      }
      ret.AppendToBody("</ul>\r\n");
    }
  }
  /*
    Finishing the response
  */
  ret.set_protocol("HTTP/1.1");
  ret.set_response_code(200);
  ret.set_message("ITS WORKING!!");     

  return ret;
}

}  // namespace hw4
