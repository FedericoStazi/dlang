// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>

enum Tag {
  Unit,
  Bool,
  Int,
  HeapIndex,
  HeapRef,
  CodeIndex,
  ReturnAddress,
  FramePointer,
  PairHeader,
  InlHeader,
  InrHeader,
  ClosureHeader
};

using Value = size_t;

// Object representing a generic item in memory
struct Item {
  Tag tag;
  Value value;
};

class MemoryManager;

// Object representing a readable and writable chunk of memory
class Memory {
 public:
  explicit Memory(size_t size = 0);
  ~Memory();

  explicit Memory(const Memory& other) = delete;
  Memory& operator=(const Memory& other);

  void copyFrom(const Memory& other);

  void setMemoryManager(std::shared_ptr<MemoryManager> memoryManager);

  // Methods used to access the underlying data
  size_t size() const;
  Item& operator[](size_t idx);

  // Methods used to access thorough the memory manager
  void checkSize(size_t idx);
  void checkTag(size_t idx, Tag tag);
  Item get(size_t idx);
  Item getAndCheck(size_t idx, Tag tag);
  void set(size_t idx, Item item);
  void allocate();

  // Methods used for Just-In-Time compilation
  size_t* getSizePtr();
  Item** getDataPtr();
  static int allocateStatic(Memory* memory);

 private:
  size_t size_;
  Item* items_;
  std::shared_ptr<MemoryManager> memoryManager_;
};
