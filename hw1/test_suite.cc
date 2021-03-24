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

#include <iostream>

#include "gtest/gtest.h"
#include "./test_suite.h"

using std::cout;
using std::endl;

// static
int HW1Environment::points_ = 0;

// static
void HW1Environment::AddPoints(int points) {
  points_ += points;
  cout << " (" << points_ << "/" << HW1_MAXPOINTS << ")"<< endl;
}

void HW1Environment::SetUp() {
  cout << "HW1: there are " << HW1_MAXPOINTS;
  cout << " points available." << endl;
}

void HW1Environment::TearDown() {
  // Code here is run once for the entire test environment.
  cout << endl;
  cout << "You earned " << points_ << " out of ";
  cout << HW1_MAXPOINTS << " points available (";
  cout << ((100.0 * points_) / HW1_MAXPOINTS) << "%)" << endl;
  cout << endl;
}


int main(int argc, char **argv) {
  ::testing::InitGoogleTest(&argc, argv);
  ::testing::AddGlobalTestEnvironment(new HW1Environment);
  return RUN_ALL_TESTS();
}
