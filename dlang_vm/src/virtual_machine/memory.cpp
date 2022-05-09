// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "memory.h"

#include <algorithm>

#include "../memory_managers/memory_manager.h"
#include "../virtual_machine/exception.h"

Memory::Memory(size_t size) : size_(size), items_(new Item[size]) {}

Memory::~Memory() {
  delete[] items_;
}

Memory& Memory::operator=(const Memory& other) {
  delete[] items_;
  size_ = other.size_;
  items_ = new Item[size_];
  std::copy(other.items_, other.items_ + size_, items_);
  return *this;
}

void Memory::copyFrom(const Memory& other) {
  std::copy(other.items_, other.items_ + std::min(size_, other.size_), items_);
}

void Memory::setMemoryManager(std::shared_ptr<MemoryManager> memoryManager) {
  memoryManager_ = memoryManager;
}

size_t Memory::size() const {
  return size_;
}

Item& Memory::operator[](size_t idx) {
  return items_[idx];
}

void Memory::checkSize(size_t idx) {
  while (idx >= size_) {
    allocate();
  }
}

void Memory::checkTag(size_t idx, Tag tag) {
  checkSize(idx);
  if (items_[idx].tag != tag) {
    throw RuntimeError();
  }
}

Item Memory::get(size_t idx) {
  checkSize(idx);
  return items_[idx];
}

Item Memory::getAndCheck(size_t idx, Tag tag) {
  checkTag(idx, tag);
  return items_[idx];
}

void Memory::set(size_t idx, Item item) {
  checkSize(idx);
  items_[idx] = item;
}

void Memory::allocate() {
  if (memoryManager_) {
    memoryManager_->allocateMemory(this);
  } else {
    throw RuntimeError();
  }
}

size_t* Memory::getSizePtr() { return &size_; }
Item** Memory::getDataPtr() { return &items_; }

int Memory::allocateStatic(Memory* memory) {
  try {
    memory->allocate();
  } catch (const RuntimeError&) {
    return 1;
  }
  return 0;
}
