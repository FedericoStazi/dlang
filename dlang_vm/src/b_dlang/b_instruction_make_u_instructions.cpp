// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "b_instruction.h"

#include "../virtual_machine/runtime_system.h"
#include "../u_dlang/u_code_builder.h"
#include "../u_dlang/u_multi_instruction.h"

Code<UInstruction> BUnary::getUInstructions(VMPtr vm) const {
  Tag tag;
  if (op_ == Read) { tag = Unit; }
  if (op_ == Not) { tag = Bool; }
  if (op_ == Neg) { tag = Int; }
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val), tag)
    .add<UUnary>(op_, VMUArg::r0, VMUArg::r0)
    // Store the result
    .add<USetAndCheck>(VMUArg::SP(-1, VMUArg::Val), VMUArg::r0)
    .addIf<USetAndCheck>(op_ == Read, VMUArg::SP(-1, VMUArg::Tag),
                         VMUArg::uImm(Int))
    // Update registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BOper::getUInstructions(VMPtr vm) const {
  Tag tagArg = (op_ == And || op_ == Or) ? Bool : Int;
  Tag tagRes = (op_ == Lt || op_ == Eq) ? Bool : tagArg;
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get arguments and apply operation
    .addIf<UGetAndCheck>(op_ != Eq, VMUArg::r0,
                         VMUArg::SP(-2, VMUArg::Val), tagArg)
    .addIf<UGetAndCheck>(op_ != Eq, VMUArg::r1,
                         VMUArg::SP(-1, VMUArg::Val), tagArg)
    .addIf<UGetAndCheck>(op_ == Eq, VMUArg::r0, VMUArg::SP(-2, VMUArg::Tag))
    .addIf<UGetAndCheck>(op_ == Eq, VMUArg::r1, VMUArg::SP(-1, VMUArg::Tag))
    .add<UOper>(op_, VMUArg::r0, VMUArg::r0, VMUArg::r1)
    // Also check tags equality for eq
    .addIf<UGetAndCheck>(op_ == Eq, VMUArg::r1, VMUArg::SP(-2, VMUArg::Val))
    .addIf<UGetAndCheck>(op_ == Eq, VMUArg::r2, VMUArg::SP(-1, VMUArg::Val))
    .addIf<UOper>(op_ == Eq, Eq, VMUArg::r1, VMUArg::r1, VMUArg::r2)
    .addIf<UOper>(op_ == Eq, And, VMUArg::r0, VMUArg::r0, VMUArg::r1)
    // Store the result
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Val), VMUArg::r0)
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Tag), VMUArg::uImm(tagRes))
    // Update registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BMkPair::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Create the heap header
    .add<USetAndCheck>(VMUArg::HP(+0, VMUArg::Tag), VMUArg::uImm(PairHeader))
    .add<USetAndCheck>(VMUArg::HP(+0, VMUArg::Val), VMUArg::uImm(3))
    // Store the item on the heap
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-2, VMUArg::Tag),
                        VMUArg::HP(+1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-2, VMUArg::Val),
                        VMUArg::HP(+1, VMUArg::Val))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::HP(+2, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::HP(+2, VMUArg::Val))
    .add<UMakeHeapPointer>(VMUArg::SP(-2, VMUArg::Val), VMUArg::uImm(HeapIndex))
    // Update the registers
    .add<UOper>(Add, VMUArg::hp, VMUArg::hp, VMUArg::uImm(3))
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BFst::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the heap pointer
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-1, VMUArg::Val), HeapIndex)
    // Check the heap tag
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag),
                       PairHeader)
    // Get the item
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +1, VMUArg::Tag),
                        VMUArg::SP(-1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +1, VMUArg::Val),
                        VMUArg::SP(-1, VMUArg::Val))
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BSnd::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the heap pointer
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-1, VMUArg::Val), HeapIndex)
    // Check the heap tag
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag),
                       PairHeader)
    // Get the item
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +2, VMUArg::Tag),
                        VMUArg::SP(-1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +2, VMUArg::Val),
                        VMUArg::SP(-1, VMUArg::Val))
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BMkInl::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Create the heap header
    .add<USetAndCheck>(VMUArg::HP(0, VMUArg::Tag), VMUArg::uImm(InlHeader))
    .add<USetAndCheck>(VMUArg::HP(0, VMUArg::Val), VMUArg::uImm(2))
    // Store the item on the heap
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::HP(+1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::HP(+1, VMUArg::Val))
    .add<UMakeHeapPointer>(VMUArg::SP(-1, VMUArg::Val), VMUArg::uImm(HeapIndex))
    // Update the registers
    .add<UOper>(Add, VMUArg::hp, VMUArg::hp, VMUArg::uImm(2))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BMkInr::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Create the heap header
    .add<USetAndCheck>(VMUArg::HP(0, VMUArg::Tag), VMUArg::uImm(InrHeader))
    .add<USetAndCheck>(VMUArg::HP(0, VMUArg::Val), VMUArg::uImm(2))
    // Store the item on the heap
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::HP(+1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::HP(+1, VMUArg::Val))
    .add<UMakeHeapPointer>(VMUArg::SP(-1, VMUArg::Val), VMUArg::uImm(HeapIndex))
    // Update the registers
    .add<UOper>(Add, VMUArg::hp, VMUArg::hp, VMUArg::uImm(2))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BPush::getUInstructions(VMPtr vm) const {
  ::Tag tag;
  if (tag_ == Unit) { tag = ::Unit; }
  if (tag_ == Bool) { tag = ::Bool; }
  if (tag_ == Int) { tag = ::Int; }
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Create the item
    .add<USetAndCheck>(VMUArg::SP(+0, VMUArg::Tag), VMUArg::uImm(tag))
    .add<USetAndCheck>(VMUArg::SP(+0, VMUArg::Val), VMUArg::uImm(value_))
    // Update the registers
    .add<UOper>(Add, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BApply::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Store the frame pointer
    .add<USetAndCheck>(VMUArg::SP(+0, VMUArg::Tag), VMUArg::uImm(FramePointer))
    .add<USetAndCheck>(VMUArg::SP(+0, VMUArg::Val), VMUArg::fp)
    // Store the return address
    .add<USetAndCheck>(VMUArg::SP(+1, VMUArg::Tag), VMUArg::uImm(ReturnAddress))
    .add<UOper>(BOper::Op::Add, VMUArg::r0, VMUArg::cp, VMUArg::uImm(+1))
    .add<USetAndCheck>(VMUArg::SP(+1, VMUArg::Val), VMUArg::r0)
    // Get the heap pointer
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-1, VMUArg::Val), HeapIndex)
    // Check the code pointer tag
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +1, VMUArg::Val),
                       CodeIndex)
    // Update the registers
    .add<UOper>(Add, VMUArg::fp, VMUArg::sp, VMUArg::uImm(0))
    .add<UOper>(Add, VMUArg::sp, VMUArg::sp, VMUArg::uImm(2))
    .add<UMove>(VMUArg::cp, VMUArg::r0)
    .add<UApply>()
    .add<UGuard>();
}

Code<UInstruction> BLookup::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the heap pointer
    .addIf<UGetAndCheck>(location_ == Location::Heap, VMUArg::r2,
                         VMUArg::FP(-1, VMUArg::Val), HeapIndex)
    // Get the item
    .addIf<UMoveAndCheck>(location_ == Location::Stack, VMUArg::r0,
                          VMUArg::FP(+offset_, VMUArg::Tag),
                          VMUArg::SP(+0, VMUArg::Tag))
    .addIf<UMoveAndCheck>(location_ == Location::Stack, VMUArg::r0,
                          VMUArg::FP(+offset_, VMUArg::Val),
                          VMUArg::SP(+0, VMUArg::Val))
    .addIf<UMoveAndCheck>(location_ == Location::Heap, VMUArg::r0,
                          VMUArg::Heap(VMUArg::r2, +offset_ + 1,
                          VMUArg::Tag), VMUArg::SP(+0, VMUArg::Tag))
    .addIf<UMoveAndCheck>(location_ == Location::Heap, VMUArg::r0,
                          VMUArg::Heap(VMUArg::r2, +offset_ + 1,
                          VMUArg::Val), VMUArg::SP(+0, VMUArg::Val))
    // Update the registers
    .add<UOper>(Add, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BReturn::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Store the return value
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::FP(-2, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::FP(-2, VMUArg::Val))
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::FP(+0, VMUArg::Val), FramePointer)
    .add<UGetAndCheck>(VMUArg::r1, VMUArg::FP(+1, VMUArg::Val), ReturnAddress)
    // Update the registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::fp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::fp, VMUArg::r0, VMUArg::uImm(0))
    .add<UMove>(VMUArg::cp, VMUArg::r1)
    .add<UReturn>()
    .add<UGuard>();
}

Code<UInstruction> BMkClosure::getUInstructions(VMPtr vm) const {
  Code<UInstruction> uCode = UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Create the heap header
    .add<USetAndCheck>(VMUArg::HP(+0, VMUArg::Tag), VMUArg::uImm(ClosureHeader))
    .add<USetAndCheck>(VMUArg::HP(+0, VMUArg::Val), VMUArg::uImm(2 + size_))
    // Store the code pointer on the heap
    .add<USetAndCheck>(VMUArg::HP(+1, VMUArg::Tag), VMUArg::uImm(CodeIndex))
    .add<USetAndCheck>(VMUArg::HP(+1, VMUArg::Val), VMUArg::uImm(location_));

  for (int i = 0; i < size_; i++) {
    uCode += UCodeBuilder(getCp(), vm)
      // Store the variables on the heap
      .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1 - i, VMUArg::Tag),
                          VMUArg::HP(+2 + i, VMUArg::Tag))
      .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1 - i, VMUArg::Val),
                          VMUArg::HP(+2 + i, VMUArg::Val));
  }

  uCode += UCodeBuilder(getCp(), vm)
    // Store the heap pointer
    .add<UMakeHeapPointer>(VMUArg::SP(-size_, VMUArg::Val),
                           VMUArg::uImm(HeapIndex))
    // Update the registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(size_ - 1))
    .add<UOper>(Add, VMUArg::hp, VMUArg::hp, VMUArg::uImm(size_ + 2))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));

  return uCode;
}

Code<UInstruction> BSwap::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Swap the tags
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::SP(-2, VMUArg::Tag))
    .add<UGetAndCheck>(VMUArg::r1, VMUArg::SP(-1, VMUArg::Tag))
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Tag), VMUArg::r1)
    .add<USetAndCheck>(VMUArg::SP(-1, VMUArg::Tag), VMUArg::r0)
    // Swap the values
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::SP(-2, VMUArg::Val))
    .add<UGetAndCheck>(VMUArg::r1, VMUArg::SP(-1, VMUArg::Val))
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Val), VMUArg::r1)
    .add<USetAndCheck>(VMUArg::SP(-1, VMUArg::Val), VMUArg::r0)
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BPop::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Update the registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BLabel::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BFunction::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BDeref::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-1, VMUArg::Val), HeapRef)
    // Get the item
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag),
                        VMUArg::SP(-1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +0, VMUArg::Val),
                        VMUArg::SP(-1, VMUArg::Val))
    // Update the registers
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BMkRef::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Store the item on the heap
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::HP(+0, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::HP(+0, VMUArg::Val))
    .add<UMakeHeapPointer>(VMUArg::SP(-1, VMUArg::Val), VMUArg::uImm(HeapRef))
    // Update the registers
    .add<UOper>(Add, VMUArg::hp, VMUArg::hp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BAssign::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the heap pointer
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-2, VMUArg::Val), HeapRef)
    // Store the item on the heap
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Tag),
                        VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val),
                        VMUArg::Heap(VMUArg::r2, +0, VMUArg::Val))
    // Store unit on the stack
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Tag), VMUArg::uImm(Unit))
    .add<USetAndCheck>(VMUArg::SP(-2, VMUArg::Val), VMUArg::uImm(0))
    // Update the registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    .add<UOper>(Add, VMUArg::cp, VMUArg::cp, VMUArg::uImm(1));
}

Code<UInstruction> BHalt::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    .add<UHalt>();
}

Code<UInstruction> BGoto::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    .add<UGoto>(destination_)
    .add<UGuard>();
}

Code<UInstruction> BTest::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the argument
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::SP(-1, VMUArg::Val), Bool)
    // Update the registers
    .add<UOper>(Sub, VMUArg::sp, VMUArg::sp, VMUArg::uImm(1))
    // Branch
    .add<UBranch>(VMUArg::r0, destination_)
    .add<UGuard>();
}

Code<UInstruction> BCase::getUInstructions(VMPtr vm) const {
  return UCodeBuilder(getCp(), vm)
    .add<ULabel>()
    // Get the heap pointer
    .add<UGetAndCheck>(VMUArg::r2, VMUArg::SP(-1, VMUArg::Val), HeapIndex)
    // Check the heap tag
    .add<UTagCheck>(VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag),
                    InlHeader, InrHeader)
    // Get the item
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +1, VMUArg::Tag),
                        VMUArg::SP(-1, VMUArg::Tag))
    .add<UMoveAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +1, VMUArg::Val),
                        VMUArg::SP(-1, VMUArg::Val))
    // Branch based on the tag
    .add<UGetAndCheck>(VMUArg::r0, VMUArg::Heap(VMUArg::r2, +0, VMUArg::Tag))
    .add<UOper>(BOper::Op::Eq, VMUArg::r0, VMUArg::r0, VMUArg::uImm(InlHeader))
    .add<UBranch>(VMUArg::r0, destination_)
    .add<UGuard>();
}
