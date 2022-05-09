// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include <gtest/gtest.h>

#include "../../src/data_structures/code.h"

TEST(Code, GetInstruction) {
  // Create a Code<int> object (sequence of ints) {1}
  Code<int> code;
  code.add(std::make_shared<int>(1));

  // Test getInstruction method
  EXPECT_EQ(*code.getInstruction(0), 1);
  EXPECT_ANY_THROW(code.getInstruction(1));
}

TEST(Code, Iteration) {
  // Create a Code<int> object (sequence of ints) {1, 2, 3}
  Code<int> code;
  code.add(std::make_shared<int>(1));
  code.add(std::make_shared<int>(2));
  code.add(std::make_shared<int>(3));

  // Iterate and collect values
  std::vector<int> values;
  for (const auto& x : code) {
    values.push_back(*x);
  }

  // Test values
  EXPECT_EQ(values.at(0), 1);
  EXPECT_EQ(values.at(1), 2);
  EXPECT_EQ(values.at(2), 3);
}

TEST(Code, AddCode) {
  // Create two Code<int> object (sequences of ints) {1, 2} {3, 4}
  Code<int> codeA, codeB;
  codeA.add(std::make_shared<int>(1));
  codeA.add(std::make_shared<int>(2));
  codeB.add(std::make_shared<int>(3));
  codeB.add(std::make_shared<int>(4));

  // Add b to a
  codeA.add(std::make_shared<Code<int>>(codeB));

  // Test values
  EXPECT_EQ(*codeA.getInstruction(0), 1);
  EXPECT_EQ(*codeA.getInstruction(1), 2);
  EXPECT_EQ(*codeA.getInstruction(2), 3);
  EXPECT_EQ(*codeA.getInstruction(3), 4);
}

TEST(Code, Plus) {
  // Create two Code<int> object (sequences of ints) {1, 2} {3, 4}
  Code<int> codeA, codeB;
  codeA.add(std::make_shared<int>(1));
  codeA.add(std::make_shared<int>(2));
  codeB.add(std::make_shared<int>(3));
  codeB.add(std::make_shared<int>(4));

  // Add a and b
  auto codeC = codeA + codeB;

  // Test values
  EXPECT_EQ(*codeC.getInstruction(0), 1);
  EXPECT_EQ(*codeC.getInstruction(1), 2);
  EXPECT_EQ(*codeC.getInstruction(2), 3);
  EXPECT_EQ(*codeC.getInstruction(3), 4);
}

TEST(Code, PlusEquals) {
  // Create two Code<int> object (sequences of ints) {1, 2} {3, 4}
  Code<int> codeA, codeB;
  codeA.add(std::make_shared<int>(1));
  codeA.add(std::make_shared<int>(2));
  codeB.add(std::make_shared<int>(3));
  codeB.add(std::make_shared<int>(4));

  // Add b to a
  codeA += codeB;

  // Test values
  EXPECT_EQ(*codeA.getInstruction(0), 1);
  EXPECT_EQ(*codeA.getInstruction(1), 2);
  EXPECT_EQ(*codeA.getInstruction(2), 3);
  EXPECT_EQ(*codeA.getInstruction(3), 4);
}
