// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "b_instruction.h"

#include "../virtual_machine/runtime_system.h"

void BUnary::interpret(VMPtr vm) const {
  auto a = vm->stack.get(vm->sp - 1);
  if (op_ == Not && a.tag == Tag::Bool) {
    vm->stack.set(vm->sp - 1, {a.tag, !static_cast<bool>(a.value)});
  } else if (op_ == Neg && a.tag == Tag::Int) {
    vm->stack.set(vm->sp - 1,
                  {a.tag, static_cast<size_t>(-static_cast<int>(a.value))});
  } else if (op_ == Read && a.tag == Tag::Unit) {
    vm->stack.set(vm->sp - 1,
                  {Tag::Int, static_cast<size_t>(RuntimeSystem::readInt())});
  } else {
    throw RuntimeError();
  }
  vm->cp++;
}

void BOper::interpret(VMPtr vm) const {
  auto a = vm->stack.get(vm->sp - 2);
  auto b = vm->stack.get(vm->sp - 1);
  if (op_ == And && a.tag == Tag::Bool && b.tag == Tag::Bool) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, static_cast<bool>(a.value) &&
                                          static_cast<bool>(b.value)});
  } else if (op_ == Or && a.tag == Tag::Bool && b.tag == Tag::Bool) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, static_cast<bool>(a.value) ||
                                          static_cast<bool>(b.value)});
  } else if (op_ == Eq && a.tag == Tag::Unit && b.tag == Tag::Unit) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, true});
  } else if (op_ == Eq && a.tag == b.tag) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, static_cast<int>(a.value) ==
                                          static_cast<int>(b.value)});
  } else if (op_ == Eq && a.tag != b.tag) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, false});
  } else if (op_ == Lt && a.tag == Tag::Int && b.tag == Tag::Int) {
    vm->stack.set(vm->sp - 2, {Tag::Bool, static_cast<int>(a.value) <
                                          static_cast<int>(b.value)});
  } else if (op_ == Add && a.tag == Tag::Int && b.tag == Tag::Int) {
    vm->stack.set(vm->sp - 2, {Tag::Int,
                             static_cast<size_t>((static_cast<int>(a.value) +
                                                  static_cast<int>(b.value)))});
  } else if (op_ == Sub && a.tag == Tag::Int && b.tag == Tag::Int) {
    vm->stack.set(vm->sp - 2, {Tag::Int,
                             static_cast<size_t>((static_cast<int>(a.value) -
                                                  static_cast<int>(b.value)))});
  } else if (op_ == Mul && a.tag == Tag::Int && b.tag == Tag::Int) {
    vm->stack.set(vm->sp - 2, {Tag::Int,
                             static_cast<size_t>((static_cast<int>(a.value) *
                                                  static_cast<int>(b.value)))});
  } else if (op_ == Div && a.tag == Tag::Int && b.tag == Tag::Int) {
    if (b.value == 0) { throw RuntimeError(); }
    vm->stack.set(vm->sp - 2, {Tag::Int,
                             static_cast<size_t>((static_cast<int>(a.value) /
                                                  static_cast<int>(b.value)))});
  } else {
    throw RuntimeError();
  }
  vm->cp++;
  vm->sp--;
}

void BMkPair::interpret(VMPtr vm) const {
  vm->heap.set(vm->hp, {Tag::PairHeader, 3});
  vm->heap.set(vm->hp + 1, vm->stack.get(vm->sp - 2));
  vm->heap.set(vm->hp + 2, vm->stack.get(vm->sp - 1));
  vm->stack.set(vm->sp - 2, {Tag::HeapIndex, vm->hp});
  vm->sp--;
  vm->hp += 3;
  vm->cp++;
}

void BFst::interpret(VMPtr vm) const {
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 1, Tag::HeapIndex).value;
  vm->heap.checkTag(heapIdx, Tag::PairHeader);
  vm->stack.set(vm->sp - 1, vm->heap.get(heapIdx + 1));
  vm->cp++;
}

void BSnd::interpret(VMPtr vm) const {
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 1, Tag::HeapIndex).value;
  vm->heap.checkTag(heapIdx, Tag::PairHeader);
  vm->stack.set(vm->sp - 1, vm->heap.get(heapIdx + 2));
  vm->cp++;
}

void BMkInl::interpret(VMPtr vm) const {
  vm->heap.set(vm->hp, {Tag::InlHeader, 2});
  vm->heap.set(vm->hp + 1, vm->stack.get(vm->sp - 1));
  vm->stack.set(vm->sp - 1, {Tag::HeapIndex, vm->hp});
  vm->hp += 2;
  vm->cp++;
}

void BMkInr::interpret(VMPtr vm) const {
  vm->heap.set(vm->hp, {Tag::InrHeader, 2});
  vm->heap.set(vm->hp + 1, vm->stack.get(vm->sp - 1));
  vm->stack.set(vm->sp - 1, {Tag::HeapIndex, vm->hp});
  vm->hp += 2;
  vm->cp++;
}

void BPush::interpret(VMPtr vm) const {
  if (tag_ == Unit) {
    vm->stack.set(vm->sp, {::Tag::Unit, {}});
  } else if (tag_ == Bool) {
    vm->stack.set(vm->sp, {::Tag::Bool, static_cast<bool>(value_)});
  } else if (tag_ == Int) {
    vm->stack.set(vm->sp, {::Tag::Int, static_cast<size_t>(value_)});
  }
  vm->sp++;
  vm->cp++;
}

void BApply::interpret(VMPtr vm) const {
  vm->stack.set(vm->sp, {Tag::FramePointer, vm->fp});
  vm->stack.set(vm->sp + 1, {Tag::ReturnAddress, vm->cp + 1});
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 1, Tag::HeapIndex).value;
  vm->cp = vm->heap.getAndCheck(heapIdx + 1, Tag::CodeIndex).value;
  vm->fp = vm->sp;
  vm->sp += 2;
}

void BLookup::interpret(VMPtr vm) const {
  if (location_ == Location::Stack) {
    vm->stack.set(vm->sp, vm->stack.get(vm->fp + offset_));
  } else if (location_ == Location::Heap) {
    auto heapIdx = vm->stack.getAndCheck(vm->fp - 1, Tag::HeapIndex).value;
    vm->stack.set(vm->sp, vm->heap.get(heapIdx + offset_ + 1));
  }
  vm->sp++;
  vm->cp++;
}

void BReturn::interpret(VMPtr vm) const {
  vm->stack.set(vm->fp - 2, vm->stack.get(vm->sp - 1));
  vm->sp = vm->fp - 1;
  vm->cp = vm->stack.getAndCheck(vm->fp + 1, Tag::ReturnAddress).value;
  vm->fp = vm->stack.getAndCheck(vm->fp, Tag::FramePointer).value;
}

void BMkClosure::interpret(VMPtr vm) const {
  vm->heap.set(vm->hp, {Tag::ClosureHeader, 2 + size_});
  vm->heap.set(vm->hp + 1, {Tag::CodeIndex, location_});
  for (int i = 0; i < size_; i++) {
    vm->heap.set(vm->hp + 2 + i, vm->stack.get(vm->sp - 1 - i));
  }
  vm->stack.set(vm->sp - size_, {Tag::HeapIndex, vm->hp});
  vm->sp -= size_ - 1;
  vm->hp += 2 + size_;
  vm->cp++;
}

void BSwap::interpret(VMPtr vm) const {
  auto a = vm->stack.get(vm->sp - 2);
  auto b = vm->stack.get(vm->sp - 1);
  vm->stack.set(vm->sp - 1, a);
  vm->stack.set(vm->sp - 2, b);
  vm->cp++;
}

void BPop::interpret(VMPtr vm) const {
  vm->sp--;
  vm->cp++;
}

void BLabel::interpret(VMPtr vm) const {
  vm->cp++;
}

void BFunction::interpret(VMPtr vm) const {
  vm->cp++;
}

void BDeref::interpret(VMPtr vm) const {
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 1, Tag::HeapRef).value;
  vm->stack.set(vm->sp - 1, vm->heap.get(heapIdx));
  vm->cp++;
}

void BMkRef::interpret(VMPtr vm) const {
  vm->heap.set(vm->hp, vm->stack.get(vm->sp - 1));
  vm->stack.set(vm->sp - 1, {Tag::HeapRef, vm->hp});
  vm->hp++;
  vm->cp++;
}

void BAssign::interpret(VMPtr vm) const {
  auto item = vm->stack.get(vm->sp - 1);
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 2, Tag::HeapRef).value;
  vm->heap.set(heapIdx, item);
  vm->stack.set(vm->sp - 2, {Tag::Unit, {}});
  vm->sp--;
  vm->cp++;
}

void BHalt::interpret(VMPtr vm) const {
  vm->status = VirtualMachine::Status::Halted;
}

void BGoto::interpret(VMPtr vm) const {
  vm->cp = destination_;
}

void BTest::interpret(VMPtr vm) const {
  if (vm->stack.getAndCheck(vm->sp - 1, Tag::Bool).value) {
    vm->cp++;
  } else {
    vm->cp = destination_;
  }
  vm->sp--;
}

void BCase::interpret(VMPtr vm) const {
  auto heapIdx = vm->stack.getAndCheck(vm->sp - 1, Tag::HeapIndex).value;
  auto item = vm->heap.get(heapIdx);
  vm->stack.set(vm->sp - 1, vm->heap.get(heapIdx + 1));
  if (item.tag == Tag::InlHeader) {
    vm->cp++;
  } else if (item.tag == Tag::InrHeader) {
    vm->cp = destination_;
  } else {
    throw RuntimeError();
  }
}
