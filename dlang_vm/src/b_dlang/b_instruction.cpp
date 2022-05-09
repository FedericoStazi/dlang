// Copyright 2022 Federico Stazi. Subject to the MIT license.

#include "b_instruction.h"

BInstruction::BInstruction(size_t cp) : cp_(cp) {}

size_t BInstruction::getCp() const { return cp_; }

BUnary::BUnary(size_t cp, Op op) : BInstruction(cp), op_(op) {}

BOper::BOper(size_t cp, Op op) : BInstruction(cp), op_(op) {}

BMkPair::BMkPair(size_t cp) : BInstruction(cp) {}

BFst::BFst(size_t cp) : BInstruction(cp) {}

BSnd::BSnd(size_t cp) : BInstruction(cp) {}

BMkInl::BMkInl(size_t cp) : BInstruction(cp) {}

BMkInr::BMkInr(size_t cp) : BInstruction(cp) {}

BPush::BPush(size_t cp, Tag tag, int value)
    : BInstruction(cp), tag_(tag), value_(value) {}

BApply::BApply(size_t cp) : BInstruction(cp) {}

BLookup::BLookup(size_t cp, Location location, int offset)
    : BInstruction(cp), location_(location), offset_(offset) {}

BReturn::BReturn(size_t cp) : BInstruction(cp) {}

BMkClosure::BMkClosure(size_t cp, size_t location, size_t size)
    : BInstruction(cp), location_(location), size_(size) {}

BSwap::BSwap(size_t cp) : BInstruction(cp) {}

BPop::BPop(size_t cp) : BInstruction(cp) {}

BLabel::BLabel(size_t cp) : BInstruction(cp) {}

BFunction::BFunction(size_t cp) : BInstruction(cp) {}

BDeref::BDeref(size_t cp) : BInstruction(cp) {}

BMkRef::BMkRef(size_t cp) : BInstruction(cp) {}

BAssign::BAssign(size_t cp) : BInstruction(cp) {}

BHalt::BHalt(size_t cp) : BInstruction(cp) {}

BGoto::BGoto(size_t cp, size_t destination)
    : BInstruction(cp), destination_(destination) {}

BTest::BTest(size_t cp, size_t destination)
    : BInstruction(cp), destination_(destination) {}

BCase::BCase(size_t cp, size_t destination)
    : BInstruction(cp), destination_(destination) {}
