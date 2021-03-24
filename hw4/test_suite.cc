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

#include "./test_suite.h"

#include <iostream>
#include "gtest/gtest.h"

using std::cout;
using std::endl;

// static
int HW4Environment::points_ = 0;

// static
void HW4Environment::AddPoints(int points) {
  points_ += points;
  cout << " (" << points_ << "/" << HW4_MAXPOINTS << ")"<< endl;
}

void HW4Environment::SetUp() {
  cout << "HW4: there are " << HW4_MAXPOINTS;
  cout << " points available." << endl;
}

void HW4Environment::TearDown() {
  // Code here is run once for the entire test environment.
  cout << endl;
  cout << "You earned " << points_ << " out of ";
  cout << HW4_MAXPOINTS << " points available (";
  cout << ((100.0 * points_) / HW4_MAXPOINTS) << "%)" << endl;
  cout << endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new HW4Environment);
  return RUN_ALL_TESTS();
}
