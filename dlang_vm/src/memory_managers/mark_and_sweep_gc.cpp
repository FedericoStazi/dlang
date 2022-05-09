// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "mark_and_sweep_gc.h"

#include <memory>

#include "../virtual_machine/virtual_machine.h"

MarkAndSweepGC::MarkAndSweepGC(size_t initialSize)
    : NoAllocation(initialSize) {}

void MarkAndSweepGC::collectGarbage(std::shared_ptr<VirtualMachine> vm) {
  // Use a policy to determine if collection starts
  if (10 * vm->hp < 9 * vm->heap.size()) { return; }

  // Mark phase
  marked_.clear();
  for (int idx = 0; idx < vm->sp; idx++) {
    if (vm->stack[idx].tag == HeapIndex || vm->stack[idx].tag == HeapRef) {
      markRecursive(vm->stack[idx].value, vm);
    }
  }

  // Sweep phase - creating a new compressed heap
  Memory newHeap(vm->heap.size());
  newIndex_.clear();
  size_t nextIdx = 0;
  for (size_t idx : marked_) {
    newIndex_[idx] = nextIdx;
    newHeap[nextIdx] = vm->heap[idx];
    nextIdx++;
  }
  vm->heap = newHeap;
  vm->hp = marked_.size();

  // Sweep phase - adjusting indices to the new heap
  for (int idx = 0; idx < vm->sp; idx++) {
    if (vm->stack[idx].tag == HeapIndex || vm->stack[idx].tag == HeapRef) {
      vm->stack[idx].value = newIndex_.at(vm->stack[idx].value);
    }
  }
  for (int idx = 0; idx < vm->hp; idx++) {
    if (vm->heap[idx].tag == HeapIndex || vm->heap[idx].tag == HeapRef) {
      vm->heap[idx].value = newIndex_.at(vm->heap[idx].value);
    }
  }
}

void MarkAndSweepGC::markRecursive(size_t idx,
                                   std::shared_ptr<VirtualMachine> vm) {
  if (marked_.count(idx)) { return; }
  marked_.insert(idx);

  const auto& item = vm->heap[idx];

  // Follow pointers
  if (item.tag == HeapIndex || item.tag == HeapRef) {
    markRecursive(item.value, vm);
  }

  // Mark the entire body for this header
  if (item.tag == PairHeader || item.tag == InlHeader ||
      item.tag == InrHeader || item.tag == ClosureHeader) {
    for (size_t i = 1; i < item.value; i++) {
      markRecursive(idx + i, vm);
    }
  }
}
