// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include <gtest/gtest.h>

#include "../../src/virtual_machine/memory.h"

TEST(Memory, Empty) {
  // Create an empty Memory
  Memory memory(0);

  // Test the Memory
  ASSERT_EQ(memory.size(), 0);
}

TEST(Memory, Uninitialized) {
  // Create a Memory with one element
  Memory memory(1);

  // Test the Memory
  ASSERT_EQ(memory.size(), 1);
  ASSERT_NO_THROW(memory[0]);
  ASSERT_NO_THROW(memory.get(0));
}

TEST(Memory, OneElement) {
  // Create a Memory with one element
  Memory memory(1);
  Item item{Int, 1};
  memory.set(0, item);

  ASSERT_EQ(memory.size(), 1);
  ASSERT_NO_THROW(memory.checkSize(0));
  ASSERT_ANY_THROW(memory.checkSize(1));
  ASSERT_NO_THROW(memory.checkTag(0, Int));
  ASSERT_ANY_THROW(memory.checkTag(0, Bool));
  ASSERT_EQ(memory.get(0).tag, item.tag);
  ASSERT_EQ(memory.get(0).value, item.value);
  ASSERT_NO_THROW(memory.getAndCheck(0, Int));
  ASSERT_ANY_THROW(memory.getAndCheck(0, Bool));
  ASSERT_EQ(memory.getAndCheck(0, Int).tag, item.tag);
  ASSERT_EQ(memory.getAndCheck(0, Int).value, item.value);
}

TEST(Memory, CopyFromSmall) {
  // Create two Memory with one element
  Memory memoryA(2), memoryB(1);;
  memoryA.set(0, {Unit, 0});
  memoryA.set(1, {Bool, 1});
  memoryB.set(0, {Int, 2});

  // Copy items from B to A
  memoryA.copyFrom(memoryB);

  ASSERT_EQ(memoryA.size(), 2);
  ASSERT_EQ(memoryB.size(), 1);
  ASSERT_EQ(memoryA.get(0).tag, memoryB.get(0).tag);
  ASSERT_EQ(memoryA.get(0).value, memoryB.get(0).value);
}

TEST(Memory, CopyFromLarge) {
  // Create two Memory with one element
  Memory memoryA(1), memoryB(2);;
  memoryA.set(0, {Unit, 0});
  memoryB.set(0, {Bool, 1});
  memoryB.set(1, {Int, 2});

  // Copy items from B to A
  memoryA.copyFrom(memoryB);

  ASSERT_EQ(memoryA.size(), 1);
  ASSERT_EQ(memoryB.size(), 2);
  ASSERT_EQ(memoryA.get(0).tag, memoryB.get(0).tag);
  ASSERT_EQ(memoryA.get(0).value, memoryB.get(0).value);
}
