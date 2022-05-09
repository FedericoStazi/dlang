// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "u_instruction.h"

#include "../t_dlang/t_instruction.h"

std::shared_ptr<TInstruction> UGet::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TMove>(shared_from_this(), tState->isFunction(),
                                 a->makeTArgument(tState),
                                 b->makeTArgument(tState),
                                 b->getPtr()->makeTArgument(tState));
}

std::shared_ptr<TInstruction> USet::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TMove>(shared_from_this(), tState->isFunction(),
                                 a->makeTArgument(tState),
                                 b->makeTArgument(tState),
                                 a->getPtr()->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UMove::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TMove>(shared_from_this(), tState->isFunction(),
                                 a->makeTArgument(tState),
                                 b->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UUnary::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TUnary>(shared_from_this(), tState->isFunction(),
                                 a->makeTArgument(tState),
                                 b->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UOper::getTInstruction(
    std::shared_ptr<TState> tState) {
  if (a == VMUArg::sp && b == VMUArg::sp) {
    int offset = std::dynamic_pointer_cast<UImmediate>(c)->getValue();
    if (op == Add) {
      tState->updateSp(+offset);
    } else if (op == Sub) {
      tState->updateSp(-offset);
    }
  } else if (a == VMUArg::sp) {
    tState->resetSP();
  } else if (a == VMUArg::fp) {
    tState->resetFP();
  }
  return std::make_shared<TOper>(shared_from_this(), tState->isFunction(),
                                a->makeTArgument(tState),
                                b->makeTArgument(tState),
                                c->makeTArgument(tState));
}

std::shared_ptr<TInstruction> ULabel::getTInstruction(
    std::shared_ptr<TState> tState) {
  tState->addLabel(cp);
  return std::make_shared<TLabel>(shared_from_this(), tState->isFunction());
}

std::shared_ptr<TInstruction> UGuard::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TGuard>(shared_from_this(), tState->isFunction());
}

std::shared_ptr<TInstruction> UMemCheck::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TMemCheck>(shared_from_this(),
                                     tState->isFunction(),
                                     a->makeTArgument(tState),
                                     a->getPtr()->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UTagCheck::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TTagCheck>(
      shared_from_this(),
      tState->isFunction(),
      a->makeTArgument(tState),
      tagA, tagB);
}

std::shared_ptr<TInstruction> UApply::getTInstruction(
    std::shared_ptr<TState> tState) {
  if (tState->isFunction()) {
    tState->updateSp(-3);
  }
  return std::make_shared<TApply>(
      shared_from_this(), tState->isFunction(),
      VMUArg::SP(-1, VMUArg::Tag)->makeTArgument(tState),
      VMUArg::SP(-1, VMUArg::Val)->makeTArgument(tState),
      VMUArg::SP(+0, VMUArg::Tag)->makeTArgument(tState),
      VMUArg::SP(+0, VMUArg::Val)->makeTArgument(tState),
      VMUArg::SP(+1, VMUArg::Tag)->makeTArgument(tState),
      VMUArg::SP(+1, VMUArg::Val)->makeTArgument(tState),
      VMUArg::SP(+2, VMUArg::Tag)->makeTArgument(tState),
      VMUArg::SP(+2, VMUArg::Val)->makeTArgument(tState),
      VMUArg::r0->makeTArgument(tState),
      VMUArg::r1->makeTArgument(tState),
      VMUArg::r2->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UReturn::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<TReturn>(
      shared_from_this(), tState->isFunction(),
      VMUArg::FP(-2, VMUArg::Tag)->makeTArgument(tState),
      VMUArg::FP(-2, VMUArg::Val)->makeTArgument(tState));
}

std::shared_ptr<TInstruction> UHalt::getTInstruction(
    std::shared_ptr<TState> tState) {
  return std::make_shared<THalt>(shared_from_this(), tState->isFunction());
}

std::shared_ptr<TInstruction> UGoto::getTInstruction(
    std::shared_ptr<TState> tState) {
  tState->addBranchDestination(destination);
  return std::make_shared<TGoto>(shared_from_this(),
                                 tState->isFunction(),
                                 destination);
}

std::shared_ptr<TInstruction> UBranch::getTInstruction(
    std::shared_ptr<TState> tState) {
  tState->addBranchDestination(destination);
  tState->addBranchDestination(cp + 1);
  return std::make_shared<TBranch>(shared_from_this(), tState->isFunction(),
                                   a->makeTArgument(tState),
                                   destination);
}
