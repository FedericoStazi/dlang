// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <optional>
#include <utility>
#include <vector>

class JITSequence {
 public:
  using Cps = std::vector<size_t>;
  using EntryPoints = std::vector<std::pair<size_t, size_t>>;

  JITSequence() = default;
  JITSequence(Cps cps, EntryPoints entryPoints);

  bool isEmpty() const;
  const Cps getCps() const;
  const EntryPoints getEntryPoints() const;
  void setFunction();
  bool isFunction() const;

 private:
  std::optional<Cps> cps_;
  std::optional<EntryPoints> entryPoints_;
  bool isFunction_ = false;
};
