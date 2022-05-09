// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

#include <algorithm>
#include <memory>
#include <string>

#include "t_state.h"

class UArgument;

class TArgument {
 protected:
  using UArgPtr = std::shared_ptr<UArgument>;

 public:
  using Ptr = std::shared_ptr<TArgument>;

  explicit TArgument(UArgPtr uArgument);
  UArgPtr getUArgument() const;
  virtual std::string getName() const = 0;

 private:
  UArgPtr uArgument_;
};

class TImmediate : public TArgument {
 public:
  using Ptr = std::shared_ptr<TImmediate>;

  explicit TImmediate(UArgPtr uArgument);
  virtual std::string getName() const;
};

class TVariable : public TArgument {
 public:
  using Ptr = std::shared_ptr<TVariable>;

  explicit TVariable(UArgPtr uArgument);
  TVariable(UArgPtr uArgument, size_t uid);

  virtual std::string getName() const;
  std::shared_ptr<TVariable> copy(UArgPtr uArgument);
  virtual bool operator==(const TVariable& other);
  size_t getUID() const;

 private:
  static size_t nextUid();
  const size_t uid_;
  static inline size_t nextUid_ = 0;
};
