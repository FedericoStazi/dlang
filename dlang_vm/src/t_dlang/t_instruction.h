// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <string>
#include <vector>

#include "t_argument.h"
#include "t_effects.h"
#include "../data_structures/flow_graph.h"
#include "../virtual_machine/memory.h"

class UInstruction;

class TInstruction : public std::enable_shared_from_this<TInstruction> {
 public:
  TInstruction(std::shared_ptr<UInstruction> uInstruction, bool isFunction);

  virtual std::string print() const = 0;
  virtual void makeFlowGraph(std::shared_ptr<FlowGraph<TInstruction>> graph);

  virtual std::vector<TArgument::Ptr> getReadArgs() const = 0;
  virtual std::vector<TArgument::Ptr> getWriteArgs() const = 0;

  virtual void propagateCopy(TVariable::Ptr var, TArgument::Ptr arg);
  virtual TEffects getEffects();
  virtual std::shared_ptr<TInstruction> fold();

  virtual std::shared_ptr<UInstruction> getUInstruction() const;
  bool isFunction() const;

 protected:
  void propagate(TArgument::Ptr* argOld, const TVariable::Ptr& var,
                 const TArgument::Ptr& argNew);
  std::shared_ptr<UInstruction> uInstruction;
  bool isFunction_;
};

class TInstructionBranch : public TInstruction {
 public:
  TInstructionBranch(std::shared_ptr<UInstruction> uInstruction,
                     bool isFunction, size_t destination);
  virtual void makeFlowGraph(std::shared_ptr<FlowGraph<TInstruction>> graph);
  size_t destination;
};

class TInstructionWrite : public TInstruction {
 public:
  TInstructionWrite(std::shared_ptr<UInstruction> uInstruction,
                    bool isFunction, TArgument::Ptr arg)
      : TInstruction(uInstruction, isFunction),
        var_(std::dynamic_pointer_cast<TVariable>(arg)) {}
  virtual TEffects getEffects();
  TVariable::Ptr var_;
};

class TMove : public TInstructionWrite {
 public:
  TMove(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
        TArgument::Ptr a, TArgument::Ptr b, TArgument::Ptr ptr = {});
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual void propagateCopy(TVariable::Ptr var, TArgument::Ptr arg);
  virtual std::shared_ptr<TInstruction> fold();
  virtual std::shared_ptr<UInstruction> getUInstruction() const;
  TArgument::Ptr a, b, ptr;
};

class TUnary : public TInstructionWrite {
 public:
  TUnary(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
         TArgument::Ptr a, TArgument::Ptr b);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual void propagateCopy(TVariable::Ptr var, TArgument::Ptr arg);
  virtual std::shared_ptr<TInstruction> fold();
  virtual TEffects getEffects();
  virtual std::shared_ptr<UInstruction> getUInstruction() const;
  TArgument::Ptr a, b;
};

class TOper : public TInstructionWrite {
 public:
  TOper(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
        TArgument::Ptr a, TArgument::Ptr b, TArgument::Ptr c);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual void propagateCopy(TVariable::Ptr var, TArgument::Ptr arg);
  virtual std::shared_ptr<TInstruction> fold();
  virtual TEffects getEffects();
  virtual std::shared_ptr<UInstruction> getUInstruction() const;
  TArgument::Ptr a, b, c;
};

class TLabel : public TInstruction {
 public:
  TLabel(std::shared_ptr<UInstruction> uInstruction, bool isFunction);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
};

class TGuard : public TInstruction {
 public:
  TGuard(std::shared_ptr<UInstruction> uInstruction, bool isFunction);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  void makeFlowGraph(std::shared_ptr<FlowGraph<TInstruction>> graph);
};

class TMemCheck : public TInstruction {
 public:
  TMemCheck(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
            TArgument::Ptr a, TArgument::Ptr ptr);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual TEffects getEffects();
  TArgument::Ptr a, ptr;
};

class TTagCheck : public TInstruction {
 public:
  TTagCheck(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
            TArgument::Ptr a, Tag tagA, Tag tagB);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual void propagateCopy(TVariable::Ptr var, TArgument::Ptr arg);
  virtual std::shared_ptr<TInstruction> fold();
  virtual std::shared_ptr<UInstruction> getUInstruction() const;
  virtual TEffects getEffects();
  TArgument::Ptr a;
  Tag tagA, tagB;
};

class TApply : public TInstruction {
 public:
  TApply(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
                  TArgument::Ptr argTag, TArgument::Ptr argVal,
                  TArgument::Ptr cloTag, TArgument::Ptr cloVal,
                  TArgument::Ptr fpTag, TArgument::Ptr fpVal,
                  TArgument::Ptr cpTag, TArgument::Ptr cpVal,
                  TArgument::Ptr r0, TArgument::Ptr r1, TArgument::Ptr r2);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  TArgument::Ptr argTag, argVal, cloTag, cloVal, fpTag, fpVal, cpTag, cpVal;
  TArgument::Ptr r0, r1, r2;
};

class TReturn : public TInstruction {
 public:
  TReturn(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
          TArgument::Ptr aTag, TArgument::Ptr aVal);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  TArgument::Ptr aTag, aVal;
};

class THalt : public TInstruction {
 public:
  THalt(std::shared_ptr<UInstruction> uInstruction, bool isFunction);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
};

class TGoto : public TInstructionBranch {
 public:
  TGoto(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
        size_t destination);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
};

class TBranch : public TInstructionBranch {
 public:
  TBranch(std::shared_ptr<UInstruction> uInstruction, bool isFunction,
          TArgument::Ptr a, size_t destination);
  std::string print() const;
  std::vector<TArgument::Ptr> getReadArgs() const;
  std::vector<TArgument::Ptr> getWriteArgs() const;
  virtual std::shared_ptr<TInstruction> fold();
  TArgument::Ptr a;
};
