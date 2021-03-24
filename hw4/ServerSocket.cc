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

#include <stdio.h>       // for snprintf()
#include <unistd.h>      // for close(), fcntl()
#include <sys/types.h>   // for socket(), getaddrinfo(), etc.
#include <sys/socket.h>  // for socket(), getaddrinfo(), etc.
#include <arpa/inet.h>   // for inet_ntop()
#include <netdb.h>       // for getaddrinfo()
#include <errno.h>       // for errno, used by strerror()
#include <string.h>      // for memset, strerror()
#include <iostream>      // for std::cerr, etc.

#include "./ServerSocket.h"

extern "C" {
  #include "libhw1/CSE333.h"
}

namespace hw4 {

ServerSocket::ServerSocket(uint16_t port) {
  port_ = port;
  listen_sock_fd_ = -1;
}

ServerSocket::~ServerSocket() {
  // Close the listening socket if it's not zero.  The rest of this
  // class will make sure to zero out the socket if it is closed
  // elsewhere.
  if (listen_sock_fd_ != -1)
    close(listen_sock_fd_);
  listen_sock_fd_ = -1;
}

bool ServerSocket::BindAndListen(int ai_family, int *listen_fd) {
  // Use "getaddrinfo," "socket," "bind," and "listen" to
  // create a listening socket on port port_.  Return the
  // listening socket through the output parameter "listen_fd".

  // STEP 1:

  // Structures used (as done in man getaddrinfo)
  struct addrinfo hints;  
  struct addrinfo *result, *rp;
  // sfd is socket file descriptor
  // gRes is output of getaddrinfo
  int sfd, gRes;  

  // make sure the IP address type is correct
  if(ai_family != AF_INET && ai_family != AF_INET6 && ai_family != AF_UNSPEC) {
    return false;
  }
  // zero out hints to make sure its empty
  memset(&hints, 0, sizeof(struct addrinfo)); 

  // populate hints 
  hints.ai_family = ai_family;      /* Use type defined by client */
  hints.ai_socktype = SOCK_STREAM;  /* Stream socket */
  hints.ai_flags = AI_PASSIVE;      /* For wildcard IP address */
  hints.ai_protocol = IPPROTO_TCP;  /* TCP protocol */
  hints.ai_canonname = NULL;
  hints.ai_addr = NULL;
  hints.ai_next = NULL;
  
  // Turn the port into a string
  std::string port = std::to_string(port_);
  // call getaddrinfo
  gRes = getaddrinfo(NULL,port.c_str(),&hints,&result);
  if(gRes != 0 ) {
    return false;
  }
  
  int bindSuccess = 0;
  // Since getaddrinfo() returns a list of address structures, we 
  // loop through all of them using rp->ai_next (As done in man getaddrinfo)
  for(rp = result; rp != NULL; rp = rp->ai_next) { 
    // Attempt to create a new socket 
    sfd = socket(rp->ai_family, rp->ai_socktype, rp->ai_protocol);
    if(sfd == -1) {
      // Socket creation failed, so move on in the list of addr
      continue;
    }

    // set socket option 
    int optval = 1;
    setsockopt(sfd, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    // Binding the socket by giving addr and addrlen
    if(bind(sfd, rp->ai_addr, rp->ai_addrlen) == 0)  {
      sock_family_ = rp->ai_family;
      bindSuccess = 1;
      break; // Success 
    }
    // If bind failed, close this sfd so we can reuse for next iteration
    close(sfd);
  }
  // No longer need these structs
  freeaddrinfo(result);

  // Check to see if the loop above succeeded to bind or not
  if(bindSuccess == 0) {
    return false; // We failed to bind 
  }

  // Set socket file descriptor to listen 
  if (listen(sfd, SOMAXCONN) != 0) {
    // Failed to set socket to listen
    close(sfd);
    return false;
  }

  listen_sock_fd_ = sfd;
  *listen_fd = sfd;

  return true;
}

bool ServerSocket::Accept(int *accepted_fd,
                          std::string *client_addr,
                          uint16_t *client_port,
                          std::string *client_dnsname,
                          std::string *server_addr,
                          std::string *server_dnsname) {
  // Accept a new connection on the listening socket listen_sock_fd_.
  // (Block until a new connection arrives.)  Return the newly accepted
  // socket, as well as information about both ends of the new connection,
  // through the various output parameters.

  // STEP 2:

  int client_fd; 
  struct sockaddr_storage clientaddr;
  socklen_t caddr_len = sizeof(clientaddr);
  struct sockaddr *peeraddr = reinterpret_cast<struct sockaddr *>(&clientaddr);
  // Keep attempting to accept until we either get a connection, or error
  while(1) {
    // Accepting connection
    client_fd = accept(listen_sock_fd_, peeraddr, &caddr_len);

    // If we don't get anything through accept, client_fd is negative
    if(client_fd < 0) {
      // Check if its just the case where theres no connections avaliable
      if(errno == EAGAIN) {
        continue; // Continue and try to accept again
      }
      // If some other error, return false, we failed
      return false;
    }
    // If we get this far, break the loop since we have a connection 
    break;
  }

  // Accept creates a new socket, which we now have in client_fd
  *accepted_fd = client_fd;
  // Acquire the client IP and port 
  if(peeraddr->sa_family == AF_INET) { // IPV4 

    char ip[INET_ADDRSTRLEN]; // Length of IPV4 addr
    // An internet sockaddr for IPV4 addresses
    struct sockaddr_in *ip4 = reinterpret_cast<struct sockaddr_in *>(peeraddr);
    // Read and copy from binary to text
    inet_ntop(AF_INET,&(ip4->sin_addr), ip, INET_ADDRSTRLEN);

    *client_addr = std::string(ip); // Convert ip to string, and put into output
    *client_port = htons(ip4->sin_port); // Convert to network order, put into output

  } else if(peeraddr->sa_family == AF_INET6) { // IPV6 

    char ip[INET6_ADDRSTRLEN]; // Length of IPV6 addr
    // An internet sockaddr for IPV4 addresses
    struct sockaddr_in6 *ip6 = reinterpret_cast<struct sockaddr_in6 *>(peeraddr);
    // Read and copy from binary to text
    inet_ntop(AF_INET6,&(ip6->sin6_addr), ip, INET6_ADDRSTRLEN);

    *client_addr = std::string(ip); // Convert ip to string, and put into output
    *client_port = htons(ip6->sin6_port); // Convert ip to string, and put into output

  } else {

    return false; // Couldn't identify family
  }

  char hostname[1024];
  Verify333(getnameinfo(peeraddr, caddr_len,hostname,1024,NULL,0,0) == 0);
  *client_dnsname = std::string(hostname);

  // Acquire the server IP and dns name 

  char servername[1024];

  if(sock_family_ == AF_INET) { // IPV4
    struct sockaddr_in server;
    socklen_t serverlen = sizeof(server);
    char addrbuf[INET_ADDRSTRLEN];
    // Get the socket name at client_fd
    getsockname(client_fd, (struct sockaddr *) &server, &serverlen);
    // Read the IP address from binary to text
    inet_ntop(AF_INET, &server.sin_addr, addrbuf, INET_ADDRSTRLEN);
    // Get the name of the server
    getnameinfo((const struct sockaddr *) &server,
                serverlen, servername, 1024, NULL, 0, 0);
    // Put into output parameters
    *server_addr = std::string(addrbuf);
    *server_dnsname = std::string(servername);
  } else if(sock_family_ == AF_INET6) {// IPV6
    struct sockaddr_in6 server;
    socklen_t serverlen = sizeof(server);
    char addrbuf[INET6_ADDRSTRLEN];
    // Get the socket name at client_fd
    getsockname(client_fd, (struct sockaddr *) &server, &serverlen);
    // Read the IP address from binary to text
    inet_ntop(AF_INET6, &server.sin6_addr, addrbuf, INET6_ADDRSTRLEN);
    // Get the name of the server
    getnameinfo((const struct sockaddr *) &server,
                serverlen, servername, 1024, NULL, 0, 0);
    // Put into output parameters
    *server_addr = std::string(addrbuf);
    *server_dnsname = std::string(servername);
  } else {

    return false; // Couldn't identify server family 
  }
  // Done
  return true;
}

}  // namespace hw4
