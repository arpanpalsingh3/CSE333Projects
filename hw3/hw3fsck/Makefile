# Copyright ©2020 Hal Perkins.  All rights reserved.  Permission is
# hereby granted to students registered for University of Washington
# CSE 333 for use solely during Fall Quarter 2020 for purposes of
# the course.  No other use, copying, distribution, or modification
# is permitted without prior written consent. Copyrights for
# third-party components of this work must be honored.  Instructors
# interested in reusing these course materials should contact the
# author.

AR = ar
ARFLAGS = rcs
CC = gcc
CXX = g++
CFLAGS = -g -Wall -I. -I..
LDFLAGS = -L../libhw2 -lhw2 -L../libhw1/ -lhw1
HEADERS = FileIndexChecker.h IndexTableChecker.h DocIDTableChecker.h \
          DocTableChecker.h HashTableChecker.h CheckerUtils.h FileIndexChecker.h
OBJS = ../Utils.o FileIndexChecker.o IndexTableChecker.o \
       DocIDTableChecker.o DocTableChecker.o HashTableChecker.o CheckerUtils.o

all: hw3fsck hw3fsck.a

hw3fsck: $(OBJS) hw3fsck.o
	$(CXX) $(CFLAGS) -o hw3fsck hw3fsck.o $(OBJS) $(LDFLAGS)

hw3fsck.a: $(OBJS) $(HEADERS)
	$(AR) $(ARFLAGS) hw3fsck.a $(OBJS)

%.o: %.cc $(HEADERS)
	$(CXX) $(CFLAGS) -c $<

clean:
	/bin/rm -f *.o *~ hw3fsck hw3fsck.a
