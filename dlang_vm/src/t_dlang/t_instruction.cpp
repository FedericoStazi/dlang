// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "t_instruction.h"
#include "../data_structures/flow_graph.h"
#include "../u_dlang/u_instruction.h"

TInstruction::TInstruction(std::shared_ptr<UInstruction> uInstruction,
                           bool isFunction)
    : uInstruction(uInstruction), isFunction_(isFunction) {}

void TInstruction::makeFlowGraph(
    std::shared_ptr<FlowGraph<TInstruction>> graph) {
  graph->addNodeLine(uInstruction->cp, shared_from_this());
}

void TInstruction::propagateCopy(TVariable::Ptr var, TArgument::Ptr arg) {}

TEffects TInstruction::getEffects() {
  return TEffects::makeOther();
}

std::shared_ptr<TInstruction> TInstruction::fold() {
  return shared_from_this();
}

std::shared_ptr<UInstruction> TInstruction::getUInstruction() const {
  return uInstruction;
}

bool TInstruction::isFunction() const {
  return isFunction_;
}

void TInstruction::propagate(TArgument::Ptr* argOld,
                             const TVariable::Ptr& var,
                             const TArgument::Ptr& argNew) {
  if (!std::dynamic_pointer_cast<URegSP>(var->getUArgument())) {
    if (auto varOld = std::dynamic_pointer_cast<TVariable>(*argOld)) {
      if (*var == *varOld) {
        *argOld = argNew;
      }
    }
  }
}

TInstructionBranch::TInstructionBranch(
    std::shared_ptr<UInstruction> uInstruction,
    bool isFunction, size_t destination)
    : TInstruction(uInstruction, isFunction), destination(destination) {}

void TInstructionBranch::makeFlowGraph(
    std::shared_ptr<FlowGraph<TInstruction>> graph) {
  graph->addNodeLine(uInstruction->cp, shared_from_this());
  if (isFunction()) { graph->addEdge(destination); }
}

void TGuard::makeFlowGraph(std::shared_ptr<FlowGraph<TInstruction>> graph) {
  graph->addNodeLine(uInstruction->cp, shared_from_this());
  if (isFunction()) { graph->addOutEdge(); }
}

std::shared_ptr<UInstruction> TMove::getUInstruction() const {
  std::shared_ptr<UInstruction> uInstructionNew = nullptr;
  if (auto aLoc = std::dynamic_pointer_cast<ULocation>(a->getUArgument())) {
    if (auto bOper = std::dynamic_pointer_cast<UOperand>(b->getUArgument())) {
      uInstructionNew = std::make_shared<USet>(
        uInstruction->cp, uInstruction->vm, aLoc, bOper);
    }
  }
  if (auto aReg = std::dynamic_pointer_cast<URegister>(a->getUArgument())) {
    if (auto bLoc = std::dynamic_pointer_cast<ULocation>(b->getUArgument())) {
      uInstructionNew = std::make_shared<UGet>(
        uInstruction->cp, uInstruction->vm, aReg, bLoc);
    }
  }
  if (auto aReg = std::dynamic_pointer_cast<URegister>(a->getUArgument())) {
    if (auto bOper = std::dynamic_pointer_cast<UOperand>(b->getUArgument())) {
      uInstructionNew = std::make_shared<UMove>(
        uInstruction->cp, uInstruction->vm, aReg, bOper);
    }
  }
  return uInstructionNew;
}

std::shared_ptr<UInstruction> TUnary::getUInstruction() const {
  auto uInstructionNew = std::make_shared<UUnary>(
    uInstruction->cp, uInstruction->vm,
    std::dynamic_pointer_cast<UUnary>(uInstruction)->op,
    std::dynamic_pointer_cast<URegister>(a->getUArgument()),
    std::dynamic_pointer_cast<UOperand>(b->getUArgument()));
  return uInstructionNew;
}

std::shared_ptr<UInstruction> TOper::getUInstruction() const {
  auto uInstructionNew = std::make_shared<UOper>(
    uInstruction->cp, uInstruction->vm,
    std::dynamic_pointer_cast<UOper>(uInstruction)->op,
    std::dynamic_pointer_cast<URegister>(a->getUArgument()),
    std::dynamic_pointer_cast<UOperand>(b->getUArgument()),
    std::dynamic_pointer_cast<UOperand>(c->getUArgument()));
  return uInstructionNew;
}

std::shared_ptr<UInstruction> TTagCheck::getUInstruction() const {
  auto uInstructionNew = std::make_shared<UTagCheck>(
    uInstruction->cp, uInstruction->vm,
    a->getUArgument(), tagA, tagB);
  return uInstructionNew;
}

TEffects TInstructionWrite::getEffects() {
  if (std::dynamic_pointer_cast<ULocHeap>(var_->getUArgument())) {
    return TEffects::makeOther();
  }
  if (std::dynamic_pointer_cast<ULocSP>(var_->getUArgument()) ||
      std::dynamic_pointer_cast<ULocFP>(var_->getUArgument())) {
    if (isFunction()) {
      return TEffects::makeWrite(std::dynamic_pointer_cast<TVariable>(var_));
    } else {
      return TEffects::makeOther();
    }
  }
  if (std::dynamic_pointer_cast<URegSP>(var_->getUArgument())) {
    return TEffects::makeOther();
  }
  if (std::dynamic_pointer_cast<URegGP>(var_->getUArgument())) {
    return TEffects::makeWrite(std::dynamic_pointer_cast<TVariable>(var_));
  }
}

TEffects TUnary::getEffects() {
  if (std::dynamic_pointer_cast<UUnary>(uInstruction)->op == Read) {
    return TEffects::makeOther();
  } else {
    return TInstructionWrite::getEffects();
  }
}


TEffects TOper::getEffects() {
  if (std::dynamic_pointer_cast<UOper>(uInstruction)->op == Div) {
    return TEffects::makeOther();
  } else {
    return TInstructionWrite::getEffects();
  }
}

TEffects TTagCheck::getEffects() {
  return TEffects::makeTagCheck(a, tagA, tagB);
}

TEffects TMemCheck::getEffects() {
  return TEffects::makeBoundsCheck(a);
}

TMove::TMove(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
             TArgument::Ptr a, TArgument::Ptr b, TArgument::Ptr ptr)
    : TInstructionWrite(uInstruction, isFunction, a), a(a), b(b), ptr(ptr) {}

std::vector<TArgument::Ptr> TMove::getReadArgs() const {
  if (ptr) {
    return {b, ptr};
  } else {
    return {b};
  }
}

std::vector<TArgument::Ptr> TMove::getWriteArgs() const {
  return {a};
}

void TMove::propagateCopy(TVariable::Ptr var, TArgument::Ptr arg) {
  propagate(&b, var, arg);
}

std::shared_ptr<TInstruction> TMove::fold() {
  if (auto varA = std::dynamic_pointer_cast<TVariable>(a)) {
    if (auto varB = std::dynamic_pointer_cast<TVariable>(b)) {
      if (*varA == *varB) {
        return {};
      }
    }
  }
}

TUnary::TUnary(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
               TArgument::Ptr a, TArgument::Ptr b)
    : TInstructionWrite(uInstruction, isFunction, a), a(a), b(b) {}

std::vector<TArgument::Ptr> TUnary::getReadArgs() const {
  return {b};
}

std::vector<TArgument::Ptr> TUnary::getWriteArgs() const {
  return {a};
}

void TUnary::propagateCopy(TVariable::Ptr var, TArgument::Ptr arg) {
  propagate(&b, var, arg);
}

std::shared_ptr<TInstruction> TUnary::fold() {
  if (auto bUImm = std::dynamic_pointer_cast<UImmediate>(b->getUArgument())) {
    auto op = std::dynamic_pointer_cast<UUnary>(getUInstruction())->op;
    UOperand::Ptr newB;
    if (op == Not) {
      newB = std::make_shared<UImmInt>(1 - bUImm->getValue());
    } else if (op == Neg) {
      newB = std::make_shared<UImmInt>(- bUImm->getValue());
    } else {
      return shared_from_this();
    }
    return std::make_shared<TMove>(getUInstruction(), isFunction(), a,
                                   std::make_shared<TImmediate>(newB));
  }
  return shared_from_this();
}

TOper::TOper(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
             TArgument::Ptr a, TArgument::Ptr b, TArgument::Ptr c)
    : TInstructionWrite(uInstruction, isFunction, a), a(a), b(b), c(c) {}

std::vector<TArgument::Ptr> TOper::getReadArgs() const {
  return {b, c};
}

std::vector<TArgument::Ptr> TOper::getWriteArgs() const {
  return {a};
}

void TOper::propagateCopy(TVariable::Ptr var, TArgument::Ptr arg) {
  propagate(&b, var, arg);
  propagate(&c, var, arg);
}

std::shared_ptr<TInstruction> TOper::fold() {
  if (auto bUImm = std::dynamic_pointer_cast<UImmediate>(b->getUArgument())) {
    if (auto cUImm = std::dynamic_pointer_cast<UImmediate>(c->getUArgument())) {
      auto op = std::dynamic_pointer_cast<UOper>(getUInstruction())->op;
      UOperand::Ptr newArg;
      if (op == And) {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() & cUImm->getValue());
      }
      if (op == Or)  {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() | cUImm->getValue());
      }
      if (op == Eq)  {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() == cUImm->getValue());
      }
      if (op == Lt)  {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() < cUImm->getValue());
      }
      if (op == Add) {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() + cUImm->getValue());
      }
      if (op == Sub) {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() - cUImm->getValue());
      }
      if (op == Mul) {
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() * cUImm->getValue());
      }
      if (op == Div) {
        if (!cUImm->getValue()) {
          throw OptimizationError();
        }
        newArg =
            std::make_shared<UImmInt>(bUImm->getValue() / cUImm->getValue());
      }
      return std::make_shared<TMove>(getUInstruction(), isFunction(), a,
                                    std::make_shared<TImmediate>(newArg));
    }
  }
  return shared_from_this();
}

TLabel::TLabel(std::shared_ptr<UInstruction> uInstruction, bool isFunction)
    : TInstruction(uInstruction, isFunction) {}

std::vector<TArgument::Ptr> TLabel::getReadArgs() const {
  return {};
}

std::vector<TArgument::Ptr> TLabel::getWriteArgs() const {
  return {};
}

TGuard::TGuard(std::shared_ptr<UInstruction> uInstruction, bool isFunction)
    : TInstruction(uInstruction, isFunction) {}

std::vector<TArgument::Ptr> TGuard::getReadArgs() const {
  return {};
}

std::vector<TArgument::Ptr> TGuard::getWriteArgs() const {
  return {};
}

TMemCheck::TMemCheck(std::shared_ptr<UInstruction> uInstruction,
                     bool isFunction, TArgument::Ptr a, TArgument::Ptr ptr)
    : TInstruction(uInstruction, isFunction), a(a), ptr(ptr) {}

std::vector<TArgument::Ptr> TMemCheck::getReadArgs() const {
  return {ptr};
}

std::vector<TArgument::Ptr> TMemCheck::getWriteArgs() const {
  return {};
}

TTagCheck::TTagCheck(std::shared_ptr<UInstruction> uInstruction,
                     bool isFunction, TArgument::Ptr a, Tag tagA, Tag tagB)
    : TInstruction(uInstruction, isFunction), a(a), tagA(tagA), tagB(tagB) {}

std::vector<TArgument::Ptr> TTagCheck::getReadArgs() const {
  return {a};
}

std::vector<TArgument::Ptr> TTagCheck::getWriteArgs() const {
  return {};
}

std::shared_ptr<TInstruction> TTagCheck::fold() {
  if (auto aUImm = std::dynamic_pointer_cast<UImmediate>(a->getUArgument())) {
    if (aUImm->getValue() == tagA || aUImm->getValue() == tagB) {
      return {};
    }
  }
  return shared_from_this();
}

void TTagCheck::propagateCopy(TVariable::Ptr var, TArgument::Ptr arg) {
  propagate(&a, var, arg);
}

TApply::TApply(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
                TArgument::Ptr argTag, TArgument::Ptr argVal,
                TArgument::Ptr cloTag, TArgument::Ptr cloVal,
                TArgument::Ptr fpTag, TArgument::Ptr fpVal,
                TArgument::Ptr cpTag, TArgument::Ptr cpVal,
                TArgument::Ptr r0, TArgument::Ptr r1, TArgument::Ptr r2)
    : TInstruction(uInstruction, isFunction), argTag(argTag), argVal(argVal),
      cloTag(cloTag), cloVal(cloVal), fpTag(fpTag), fpVal(fpVal),
      cpTag(cpTag), cpVal(cpVal), r0(r0), r1(r1), r2(r2) {}

std::vector<TArgument::Ptr> TApply::getReadArgs() const  {
  if (isFunction()) {
    return {argTag, argVal, cloTag, cloVal, fpTag, fpVal,
            cpTag, cpVal, r0, r1, r2};
  } else {
    return {};
  }
}

std::vector<TArgument::Ptr> TApply::getWriteArgs() const {
  if (isFunction()) {
    return {argTag, argVal, r0, r1, r2};
  } else {
    return {};
  }
}

TReturn::TReturn(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
                 TArgument::Ptr aTag, TArgument::Ptr aVal)
    : TInstruction(uInstruction, isFunction), aTag(aTag), aVal(aVal) {}

std::vector<TArgument::Ptr> TReturn::getReadArgs() const {
  if (isFunction()) {
    return {aTag, aVal};
  } else {
    return {};
  }
}

std::vector<TArgument::Ptr> TReturn::getWriteArgs() const {
  return {};
}

THalt::THalt(std::shared_ptr<UInstruction> uInstruction, bool isFunction)
    : TInstruction(uInstruction, isFunction) {}

std::vector<TArgument::Ptr> THalt::getReadArgs() const {
  return {};
}

std::vector<TArgument::Ptr> THalt::getWriteArgs() const {
  return {};
}

TGoto::TGoto(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
             size_t destination)
    : TInstructionBranch(uInstruction, isFunction, destination) {}

std::vector<TArgument::Ptr> TGoto::getReadArgs() const {
  return {};
}

std::vector<TArgument::Ptr> TGoto::getWriteArgs() const {
  return {};
}

TBranch::TBranch(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
                 TArgument::Ptr a, size_t destination)
    : TInstructionBranch(uInstruction, isFunction, destination), a(a) {}

std::vector<TArgument::Ptr> TBranch::getReadArgs() const {
  return {a};
}

std::vector<TArgument::Ptr> TBranch::getWriteArgs() const {
  return {};
}

std::shared_ptr<TInstruction> TBranch::fold() {
  if (auto aUImm = std::dynamic_pointer_cast<UImmediate>(a->getUArgument())) {
    if (aUImm->getValue()) {
      return std::make_shared<TGoto>(getUInstruction(), isFunction(),
                                     destination);
    } else {
      return {};
    }
  }
  return shared_from_this();
}
