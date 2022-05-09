// Copyright 2022 Federico Stazi. Subject to the MIT license.

#pragma once

enum BGroup {
  Individual,
  Block,
  Function
};

template <BGroup group>
class BGroupRole {
 public:
  virtual bool isStart() const;
  virtual bool isEnd() const;
  virtual bool isEntry() const;
  virtual bool isBeforeEntry() const;
 protected:
  bool isStart_ = false;
  bool isEnd_ = false;
  bool isEntry_ = false;
  bool isBeforeEntry_ = false;
};

template <BGroup group>
struct StartOf : virtual public BGroupRole<group> {
  StartOf();
};

template <BGroup group>
struct EndOf : virtual public BGroupRole<group> {
  EndOf();
};

template <BGroup group>
struct EntryOf : virtual public BGroupRole<group> {
  EntryOf();
};

template <BGroup group>
struct BeforeEntryOf : virtual public BGroupRole<group> {
  BeforeEntryOf();
};

#include "b_group.tpp"
