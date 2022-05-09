// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_instruction.h"

void UGet::jitCompile(VMPtr vm, JITPtr jit) const {
  const auto& bReg = b->getPtr()->getReg();
  jit_muli(bReg, bReg, sizeof(Item));
  jit_ldi_l(JITVM::tmp, b->getDataPtr(vm));
  if (b->getType() == VirtualMachine::Type::Val) {
    jit_addi(JITVM::tmp, JITVM::tmp,
              offsetof(Item, value) + b->getOffset() * sizeof(Item));
  }
  if (b->getType() == VirtualMachine::Type::Tag) {
    jit_addi(JITVM::tmp, JITVM::tmp,
              offsetof(Item, tag) + b->getOffset() * sizeof(Item));
  }
  // reg = (pointer + offset)->value or (pointer + offset)->tag
  jit_ldxr_l(a->getReg(), bReg, JITVM::tmp);
  if (a->getReg() != bReg) {
    jit_divi(bReg, bReg, sizeof(Item));
  }
}

void USet::jitCompile(VMPtr vm, JITPtr jit) const {
  const auto& aReg = a->getPtr()->getReg();

  // Compute the offset of the tag or value
  size_t offset = (a->getType() == VirtualMachine::Type::Val)
      ? offsetof(Item, value) + a->getOffset() * sizeof(Item)
      : offsetof(Item, tag) + a->getOffset() * sizeof(Item);

  // a = 8 * a + offset
  jit_ldi_l(JITVM::tmp, a->getDataPtr(vm));
  jit_muli(aReg, aReg, sizeof(Item));
  jit_addr(aReg, aReg, JITVM::tmp);
  jit_addi(aReg, aReg, offset);

  // a->value or a->tag = b
  if (auto bReg = std::dynamic_pointer_cast<URegister>(b)) {
    jit_str_l(aReg, bReg->getReg());
  }
  if (auto bImm = std::dynamic_pointer_cast<UImmediate>(b)) {
    jit_movi(JITVM::tmp, bImm->getValue());
    jit_str_l(aReg, JITVM::tmp);
  }

  // a = (a - offset) / 8
  jit_ldi_l(JITVM::tmp, a->getDataPtr(vm));
  jit_subi(aReg, aReg, offset);
  jit_subr(aReg, aReg, JITVM::tmp);
  jit_divi(aReg, aReg, sizeof(Item));
}

void UMove::jitCompile(VMPtr vm, JITPtr jit) const {
  if (auto bReg = std::dynamic_pointer_cast<URegister>(b)) {
    jit_movr(a->getReg(), bReg->getReg());
  }
  if (auto bImm = std::dynamic_pointer_cast<UImmediate>(b)) {
    jit_movi(a->getReg(), bImm->getValue());
  }
}

void UUnary::jitCompile(VMPtr vm, JITPtr jit) const {
  if (auto bReg = std::dynamic_pointer_cast<URegister>(b)) {
    if (op == Not) {
      jit_subi(a->getReg(), bReg->getReg(), 1);
      jit_negr(a->getReg(), a->getReg());
    } else if (op == Neg) {
      jit_negr(a->getReg(), bReg->getReg());
    } else if (op == Read) {
      JITVM::call(a->getReg(), RuntimeSystem::readInt);
    } else {
      throw InternalError();
    }
  }
  if (auto bImm = std::dynamic_pointer_cast<UImmediate>(b)) {
    if (op == Not) {
      jit_movi(a->getReg(), 1 - bImm->getValue());
    } else if (op == Neg) {
      jit_movi(a->getReg(), bImm->getValue());
      jit_negr(a->getReg(), a->getReg());
    } else if (op == Read) {
      JITVM::call(a->getReg(), RuntimeSystem::readInt);
    } else {
      throw InternalError();
    }
  }
}

void UOper::jitCompile(VMPtr vm, JITPtr jit) const {
  // Get b's register or use a temporary if it is an immediate
  jit_reg_t bb;
  if (auto bImm = std::dynamic_pointer_cast<UImmediate>(b)) {
    jit_movi(JITVM::tmp, bImm->getValue());
    bb = JITVM::tmp;
  }
  if (auto bReg = std::dynamic_pointer_cast<URegister>(b)) {
    bb = bReg->getReg();
  }

  if (auto cReg = std::dynamic_pointer_cast<URegister>(c)) {
    if (op == And) { jit_andr(a->getReg(), bb, cReg->getReg()); }
    if (op == Or)  { jit_orr (a->getReg(), bb, cReg->getReg()); }
    if (op == Eq)  { jit_eqr (a->getReg(), bb, cReg->getReg()); }
    if (op == Lt)  { jit_ltr (a->getReg(), bb, cReg->getReg()); }
    if (op == Add) { jit_addr(a->getReg(), bb, cReg->getReg()); }
    if (op == Sub) { jit_subr(a->getReg(), bb, cReg->getReg()); }
    if (op == Mul) { jit_mulr(a->getReg(), bb, cReg->getReg()); }
    if (op == Div) {
      jit->addRuntimeErrorBranch(jit_beqi(cReg->getReg(), 0));
      jit_divr(a->getReg(), bb, cReg->getReg());
    }
  }
  if (auto cImm = std::dynamic_pointer_cast<UImmediate>(c)) {
    if (op == And) { jit_andi(a->getReg(), bb, cImm->getValue()); }
    if (op == Or)  { jit_ori (a->getReg(), bb, cImm->getValue()); }
    if (op == Eq)  { jit_eqi (a->getReg(), bb, cImm->getValue()); }
    if (op == Lt)  { jit_lti (a->getReg(), bb, cImm->getValue()); }
    if (op == Add) { jit_addi(a->getReg(), bb, cImm->getValue()); }
    if (op == Sub) { jit_subi(a->getReg(), bb, cImm->getValue()); }
    if (op == Mul) { jit_muli(a->getReg(), bb, cImm->getValue()); }
    if (op == Div) {
      if (!cImm->getValue()) {
        jit->addRuntimeErrorBranch(jit_jmpi());
      }
      jit_divi(a->getReg(), bb, cImm->getValue());
    }
  }
}

void ULabel::jitCompile(VMPtr vm, JITPtr jit) const {
  jit->addLabel(cp, jit_label());
  jit->addIndirectBranch(jit_bnei(JITVM::cp, cp));
}

void UGuard::jitCompile(VMPtr vm, JITPtr jit) const {}

void UMemCheck::jitCompile(VMPtr vm, JITPtr jit) const {
  // If trying to access memory below 0, runtime error
  // If trying to access memory above max,
  //   keep allocating more memory until it gets into the bounds

  // If ptr + offset < 0, throw a runtime error
  jit->addRuntimeErrorBranch(
      jit_blti(a->getPtr()->getReg(), -a->getOffset()));

  auto checkStartLabel = jit_label();

  // If ptr + offset >= maxSize, allocate more memory
  jit_ldi_l(JITVM::tmp, a->getSizePtr(vm));
  jit_subi(JITVM::tmp, JITVM::tmp, a->getOffset());
  auto boundCheck = jit_bltr(a->getPtr()->getReg(), JITVM::tmp);

  // Failed bound check case, allocate memory
  JITVM::call(JITVM::tmp, Memory::allocateStatic, a->getMemoryPtr(vm));
  jit->addRuntimeErrorBranch(jit_bnei(JITVM::tmp, 0));

  // Jump back to the bound check
  auto allocationEndJump = jit_jmpi();
  jit_patch_at(allocationEndJump, checkStartLabel);

  jit_patch(boundCheck);
}

void UTagCheck::jitCompile(VMPtr vm, JITPtr jit) const {
  // Get a's register or use a temporary if it is an immediate or location
  jit_reg_t aa;
  if (auto aImm = std::dynamic_pointer_cast<UImmediate>(a)) {
    jit_movi(JITVM::tmp, aImm->getValue());
    aa = JITVM::tmp;
  } else if (auto aReg = std::dynamic_pointer_cast<URegister>(a)) {
    aa = aReg->getReg();
  } else if (auto aLoc = std::dynamic_pointer_cast<ULocation>(a)) {
    const auto& aReg = aLoc->getPtr()->getReg();
    jit_muli(aReg, aReg, sizeof(Item));
    jit_ldi_l(JITVM::tmp, aLoc->getDataPtr(vm));
    jit_addi(JITVM::tmp, JITVM::tmp,
             offsetof(Item, tag) + aLoc->getOffset() * sizeof(Item));
    jit_ldxr_l(JITVM::tmp, aReg, JITVM::tmp);
    jit_divi(aReg, aReg, sizeof(Item));
    aa = JITVM::tmp;
  }

  if (tagA != tagB) {
    auto branch = jit_beqi(aa, tagA);
    jit->addRuntimeErrorBranch(jit_bnei(aa, tagB));
    jit_patch(branch);
  } else {
    jit->addRuntimeErrorBranch(jit_bnei(aa, tagA));
  }
}

void UApply::jitCompile(VMPtr vm, JITPtr jit) const {}

void UReturn::jitCompile(VMPtr vm, JITPtr jit) const {}

void UHalt::jitCompile(VMPtr vm, JITPtr jit) const {
  jit_movi(JITVM::tmp, VirtualMachine::Status::Halted);
  jit_sti(&vm->status, JITVM::tmp);
}

void UGoto::jitCompile(VMPtr vm, JITPtr jit) const {
  jit_movi(JITVM::cp, destination);
  jit->addDirectBranch(jit_bnei(JITVM::cp, cp + 1), destination);
}

void UBranch::jitCompile(VMPtr vm, JITPtr jit) const {
  auto branch = jit_bnei(a->getReg(), 0);
  jit_movi(JITVM::cp, destination - 1);
  jit_patch(branch);
  jit_addi(JITVM::cp, JITVM::cp, 1);
  jit->addDirectBranch(jit_bnei(JITVM::cp, cp + 1), destination);
}
