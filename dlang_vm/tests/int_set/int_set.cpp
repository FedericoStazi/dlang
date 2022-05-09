// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include <gtest/gtest.h>

#include "../../src/data_structures/int_set.h"

TEST(IntSet, GetEmpty) {
  // Create an empty IntSet object
  IntSet a;

  // Test get method
  EXPECT_FALSE(a.get(0));
  EXPECT_FALSE(a.get(123));
}

TEST(IntSet, Get) {
  // Create an IntSet object {1,3}
  IntSet a;
  a.insert(1);
  a.insert(3);

  // Test get method
  EXPECT_FALSE(a.get(0));
  EXPECT_TRUE(a.get(1));
  EXPECT_FALSE(a.get(2));
  EXPECT_TRUE(a.get(3));
}

TEST(IntSet, Erase) {
  // Create an IntSet object {1,3}
  IntSet a;
  a.insert(1);
  a.insert(3);

  // Erase some elements
  a.erase(2);
  a.erase(3);

  // Test get method
  EXPECT_FALSE(a.get(0));
  EXPECT_TRUE(a.get(1));
  EXPECT_FALSE(a.get(2));
  EXPECT_FALSE(a.get(3));
}

TEST(IntSet, InsertSet) {
  // Create an IntSet object {1,3}
  IntSet a;
  a.insert(1);
  a.insert(3);

  // Insert another IntSet object
  IntSet b;
  b.insert(2);
  b.insert(3);
  a.insert(b);

  // Test get method
  EXPECT_FALSE(a.get(0));
  EXPECT_TRUE(a.get(1));
  EXPECT_TRUE(a.get(2));
  EXPECT_TRUE(a.get(3));
  EXPECT_FALSE(b.get(0));
  EXPECT_FALSE(b.get(1));
  EXPECT_TRUE(b.get(2));
  EXPECT_TRUE(b.get(3));
}

TEST(IntSet, Inequality) {
  // Create two IntSet object {1,3}
  IntSet a, b;
  a.insert(1);
  a.insert(3);
  b.insert(2);
  b.insert(3);

  EXPECT_TRUE(a != b);
  EXPECT_FALSE(a != a);
  EXPECT_FALSE(b != b);
}
