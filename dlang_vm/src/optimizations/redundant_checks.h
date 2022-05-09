// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <unordered_set>

#include "optimization.h"

class RemoveRedundantChecks : public OptimizationGraph {
 private:
  class ExprSet {
   public:
    explicit ExprSet(bool conj = true);
    static ExprSet makeEmpty();
    static ExprSet makeUniverse();

    void insert(const TEffects::Check& effect);
    void insert(TArgument::Ptr effect);
    void insert(const TVariable& var, int tag);
    void erase(TArgument::Ptr arg);
    bool get(const TEffects::Check& effect);
    bool get(TArgument::Ptr effect);
    void intersect(const ExprSet& other);
    bool operator!=(const ExprSet& other);

   private:
    void insert(int x);
    void erase(int x);
    bool get(int x) const;

    std::unordered_set<int> val_;
    bool conj_ = true;
    static constexpr size_t kNumTags_ = 20;
  };

  virtual void optimizeGraph(GraphPtr graph);
};
