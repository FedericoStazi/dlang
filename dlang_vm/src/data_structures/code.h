// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <memory>
#include <vector>

template<typename T>
class Code {
 public:
  Code();

  std::shared_ptr<T> getInstruction(int index) const;

  size_t size() const;
  auto begin() const;
  auto end() const;

  void add(std::shared_ptr<T> instruction);
  void add(const std::shared_ptr<Code<T>>& code);

  const Code<T>& operator+(const Code<T>& code);
  const Code<T>& operator+=(const Code<T>& code);

 private:
  std::vector<std::shared_ptr<T>> instructions_;
};

#include "code.tpp"
