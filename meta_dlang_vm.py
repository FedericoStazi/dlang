#!/usr/bin/env python2

# Copyright 2022 Federico Stazi. Subject to the MIT license.

"""DLANG byte-code interpreter"""

import os
import sys
import time

# RPython setup
from rpython.rlib.jit import JitDriver
JIT_DRIVER = JitDriver(
    greens=['cp', 'verbosity', 'code'],
    reds=['start_time', 'vm', 'stdin', 'cp_from_label'])

def jitpolicy(_):
  """The policy for JIT compilation"""
  from rpython.jit.codewriter.policy import JitPolicy
  return JitPolicy()


class DlangRuntimeError(Exception):
  """Error thrown when operations fail at run-time"""
  pass


class Tag(object):
  """The item tags"""
  Unit = 1
  Bool = 2
  Int = 3
  HeapIndex = 4
  HeapRef = 5
  CodeIndex = 6
  ReturnAddress = 7
  FramePointer = 8
  PairHeader = 9
  InlHeader = 10
  InrHeader = 11
  ClosureHeader = 12
  Unknown = 0

  @staticmethod
  def to_str(tag):
    """Convert a tag to its string representation"""
    tag_str = "Unknown"
    if   tag == Tag.Unit:
      tag_str = "Unit"
    elif tag == Tag.Bool:
      tag_str = "Bool"
    elif tag == Tag.Int:
      tag_str = "Int"
    elif tag == Tag.HeapIndex:
      tag_str = "HeapIndex"
    elif tag == Tag.HeapRef:
      tag_str = "HeapRef"
    elif tag == Tag.CodeIndex:
      tag_str = "CodeIndex"
    elif tag == Tag.ReturnAddress:
      tag_str = "ReturnAddress"
    elif tag == Tag.FramePointer:
      tag_str = "FramePointer"
    elif tag == Tag.PairHeader:
      tag_str = "PairHeader"
    elif tag == Tag.InlHeader:
      tag_str = "InlHeader"
    elif tag == Tag.InrHeader:
      tag_str = "InrHeader"
    elif tag == Tag.ClosureHeader:
      tag_str = "ClosureHeader"
    return tag_str


class Item(object):
  """The virtual machine items"""
  def __init__(self, tag=Tag.Unknown, val=0, ptr=None, bdy=[]):
    self.tag = tag
    self.val = val
    self.ptr = ptr
    self.bdy = bdy

  @staticmethod
  def to_str(item):
    """Convert an item to its string representation"""
    if item.tag in [Tag.HeapIndex, Tag.HeapRef]:
      return "(" + Tag.to_str(item.tag) + ", " + Item.to_str(item.ptr) + ")"
    elif item.tag in [Tag.PairHeader, Tag.InlHeader,
                      Tag.InrHeader, Tag.ClosureHeader]:
      return ("(" + Tag.to_str(item.tag) + ", [" +
              ", ".join([Item.to_str(x) for x in item.bdy]) + "])")
    elif item.tag in [Tag.Unknown]:
      return "(?)"
    return "(" + Tag.to_str(item.tag) + ", " + str(item.val) + ")"

  @staticmethod
  def equal(item_a, item_b):
    """Checks it two items are equal"""
    return (item_a.tag == item_b.tag and
            item_a.val == item_b.val and
            item_a.ptr is item_b.ptr and
            (item_a.bdy is item_b.bdy or
             (item_a.bdy == [] and item_b.bdy == [])))


class VirtualMachine(object):
  """Class representing the virtual machine state"""
  def __init__(self):
    self.stack = []
    self.cp = 0
    self.fp = 0
    self.status = 0

  def print_result(self):
    """Return the string representation of the virtual machine"""
    return self._print_result(self.stack[-1])

  def _print_result(self, res):
    """Return the final result of the computation in the virtual machine"""
    res_str = "ERROR"
    if res.tag == Tag.Unit:
      res_str = "()"
    if res.tag == Tag.Int:
      res_str = str(res.val)
    if res.tag == Tag.Bool and res.val == 0:
      res_str = "false"
    if res.tag == Tag.Bool and res.val == 1:
      res_str = "true"
    if res.tag == Tag.HeapRef:
      res_str = "reference"
    if res.tag == Tag.HeapIndex:
      if res.ptr.tag == Tag.PairHeader:
        str_a = self._print_result(res.ptr.bdy[0])
        str_b = self._print_result(res.ptr.bdy[1])
        if str_a != "ERROR" and str_b != "ERROR":
          res_str = "(" + str_a + ", " + str_b + ")"
    return res_str


def to_int(value):
  """Convert a value to its 32 bits representation"""
  return (value & 0xffffffff) - ((value & 0x80000000) << 1)


def interpret(code, verbosity):
  """Interpret the DLANG code"""

  # Prepare the Virtual Machine
  vm = VirtualMachine()
  vm.stack.append(Item(Tag.FramePointer, val=0))
  vm.stack.append(Item(Tag.ReturnAddress, val=0))

  # Tokenize code
  code = [line.split(" ") for line in code.split("\n")]

  # Locate the labels
  cp_from_label = {}
  for cp in range(len(code)):
    if code[cp][0] == "LABEL" or code[cp][0] == "FUNCTION":
      cp_from_label[code[cp][1]] = cp

  # Get a pointer to stdin
  stdin = os.fdopen(0)

  # Get the start time
  start_time = int(1000000000 * time.time())

  while vm.status == 0:

    if verbosity == "debug":
      print "cp = " + str(vm.cp)
      print "stack = [" + "\n   ".join([Item.to_str(x) for x in vm.stack]) + "]"

    JIT_DRIVER.jit_merge_point(cp=vm.cp, verbosity=verbosity, code=code, vm=vm,
                               stdin=stdin, start_time=start_time,
                               cp_from_label=cp_from_label)

    try:

      if code[vm.cp][0] == "UNARY":
        a = vm.stack.pop()
        if code[vm.cp][1] == "NOT" and a.tag == Tag.Bool:
          vm.stack.append(Item(Tag.Bool, val=1 - a.val))
        elif code[vm.cp][1] == "NEG" and a.tag == Tag.Int:
          vm.stack.append(Item(Tag.Int, val=to_int(-a.val)))
        elif code[vm.cp][1] == "READ" and a.tag == Tag.Unit:
          vm.stack.append(Item(Tag.Int, val=int(stdin.readline()[:-1])))
        else:
          raise DlangRuntimeError
        vm.cp += 1
        continue

      if code[vm.cp][0] == "OPER":
        b = vm.stack.pop()
        a = vm.stack.pop()
        if code[vm.cp][1] == "AND" and a.tag == Tag.Bool and b.tag == Tag.Bool:
          vm.stack.append(Item(Tag.Bool, val=1 if a.val and b.val else 0))
        elif code[vm.cp][1] == "OR" and a.tag == Tag.Bool and b.tag == Tag.Bool:
          vm.stack.append(Item(Tag.Bool, val=1 if a.val or b.val else 0))
        elif code[vm.cp][1] == "EQ":
          vm.stack.append(Item(Tag.Bool, val=1 if Item.equal(a, b) else 0))
        elif code[vm.cp][1] == "LT" and a.tag == Tag.Int and b.tag == Tag.Int:
          vm.stack.append(Item(Tag.Bool, val=1 if a.val < b.val else 0))
        elif code[vm.cp][1] == "ADD" and a.tag == Tag.Int and b.tag == Tag.Int:
          vm.stack.append(Item(Tag.Int, val=to_int(a.val + b.val)))
        elif code[vm.cp][1] == "SUB" and a.tag == Tag.Int and b.tag == Tag.Int:
          vm.stack.append(Item(Tag.Int, val=to_int(a.val - b.val)))
        elif code[vm.cp][1] == "MUL" and a.tag == Tag.Int and b.tag == Tag.Int:
          vm.stack.append(Item(Tag.Int, val=to_int(a.val * b.val)))
        elif code[vm.cp][1] == "DIV" and a.tag == Tag.Int and b.tag == Tag.Int:
          if not b.val:
            raise DlangRuntimeError
          vm.stack.append(Item(Tag.Int, val=a.val / b.val))
        else:
          raise DlangRuntimeError
        vm.cp += 1
        continue

      if code[vm.cp][0] == "MK_PAIR":
        b = vm.stack.pop()
        a = vm.stack.pop()
        pair = Item(Tag.PairHeader, bdy=[a, b])
        vm.stack.append(Item(Tag.HeapIndex, ptr=pair))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "FST":
        ptr = vm.stack.pop()
        if ptr.tag != Tag.HeapIndex:
          raise DlangRuntimeError
        pair = ptr.ptr
        if pair.tag != Tag.PairHeader:
          raise DlangRuntimeError
        vm.stack.append(pair.bdy[0])
        vm.cp += 1
        continue

      if code[vm.cp][0] == "SND":
        ptr = vm.stack.pop()
        if ptr.tag != Tag.HeapIndex:
          raise DlangRuntimeError
        pair = ptr.ptr
        if pair.tag != Tag.PairHeader:
          raise DlangRuntimeError
        vm.stack.append(pair.bdy[1])
        vm.cp += 1
        continue

      if code[vm.cp][0] == "MK_INL":
        inl = Item(Tag.InlHeader, bdy=[vm.stack.pop()])
        vm.stack.append(Item(Tag.HeapIndex, ptr=inl))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "MK_INR":
        inr = Item(Tag.InrHeader, bdy=[vm.stack.pop()])
        vm.stack.append(Item(Tag.HeapIndex, ptr=inr))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "PUSH":
        tag = code[vm.cp][1]
        if tag == "STACK_UNIT":
          vm.stack.append(Item(Tag.Unit))
        elif tag == "STACK_BOOL":
          val = 0
          if code[vm.cp][2] == "true":
            val = 1
          if code[vm.cp][2] == "false":
            val = 0
          vm.stack.append(Item(Tag.Bool, val=val))
        elif tag == "STACK_INT":
          vm.stack.append(Item(Tag.Int, val=int(code[vm.cp][2])))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "APPLY":
        vm.stack.append(Item(Tag.FramePointer, val=vm.fp))
        vm.stack.append(Item(Tag.ReturnAddress, val=vm.cp + 1))
        ptr = vm.stack[-3]
        if ptr.tag != Tag.HeapIndex:
          raise DlangRuntimeError
        closure = ptr.ptr
        if closure.tag != Tag.ClosureHeader:
          raise DlangRuntimeError
        cp = closure.bdy[0]
        if cp.tag != Tag.CodeIndex:
          raise DlangRuntimeError
        old_cp = vm.cp
        vm.cp = cp.val
        vm.fp = len(vm.stack) - 2
        if old_cp < vm.cp:
          JIT_DRIVER.can_enter_jit(
              cp=vm.cp, verbosity=verbosity, code=code,
              cp_from_label=cp_from_label, vm=vm, stdin=stdin,
              start_time=start_time)
        continue

      if code[vm.cp][0] == "LOOKUP":
        if code[vm.cp][1] == "STACK_LOCATION":
          vm.stack.append(vm.stack[vm.fp + int(code[vm.cp][2])])
        elif code[vm.cp][1] == "HEAP_LOCATION":
          ptr = vm.stack[vm.fp - 1]
          if ptr.tag != Tag.HeapIndex:
            raise DlangRuntimeError
          vm.stack.append(ptr.ptr.bdy[int(code[vm.cp][2])])
        vm.cp += 1
        continue

      if code[vm.cp][0] == "RETURN":
        item = vm.stack.pop()
        cp = vm.stack[vm.fp + 1]
        if cp.tag != Tag.ReturnAddress:
          raise DlangRuntimeError
        old_cp = vm.cp
        vm.cp = cp.val
        fp = vm.stack[vm.fp]
        if fp.tag != Tag.FramePointer:
          raise DlangRuntimeError
        if vm.fp - 2 < 0:
          raise DlangRuntimeError
        vm.stack = vm.stack[:max(0, vm.fp - 2)]
        vm.stack.append(item)
        vm.fp = fp.val
        if old_cp < vm.cp:
          JIT_DRIVER.can_enter_jit(
              cp=vm.cp, verbosity=verbosity, code=code,
              cp_from_label=cp_from_label, vm=vm, stdin=stdin,
              start_time=start_time)
        continue

      if code[vm.cp][0] == "MK_CLOSURE":
        size = int(code[vm.cp][2])
        closure = Item(Tag.ClosureHeader, bdy=[])

        closure.bdy.append(Item(Tag.CodeIndex,
                                val=cp_from_label[code[vm.cp][1]]))
        for _ in range(size):
          closure.bdy.append(vm.stack.pop())

        vm.stack.append(Item(Tag.HeapIndex, ptr=closure))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "SWAP":
        a = vm.stack.pop()
        b = vm.stack.pop()
        vm.stack.append(a)
        vm.stack.append(b)
        vm.cp += 1
        continue

      if code[vm.cp][0] == "POP":
        vm.stack.pop()
        vm.cp += 1
        continue

      if code[vm.cp][0] == "LABEL":
        vm.cp += 1
        continue

      if code[vm.cp][0] == "FUNCTION":
        vm.cp += 1
        continue

      if code[vm.cp][0] == "DEREF":
        item = vm.stack.pop()
        if item.tag != Tag.HeapRef:
          raise DlangRuntimeError
        vm.stack.append(item.ptr)
        vm.cp += 1
        continue

      if code[vm.cp][0] == "MK_REF":
        vm.stack.append(Item(Tag.HeapRef, ptr=vm.stack.pop()))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "ASSIGN":
        item = vm.stack.pop()
        ptr = vm.stack.pop()
        if ptr.tag != Tag.HeapRef:
          raise DlangRuntimeError
        ptr.ptr = item
        vm.stack.append(Item(Tag.Unit))
        vm.cp += 1
        continue

      if code[vm.cp][0] == "HALT":
        vm.status = 1
        continue

      if code[vm.cp][0] == "GOTO":
        old_cp = vm.cp
        vm.cp = cp_from_label[code[vm.cp][1]]
        if old_cp < vm.cp:
          JIT_DRIVER.can_enter_jit(
              cp=vm.cp, verbosity=verbosity, code=code,
              cp_from_label=cp_from_label, vm=vm, stdin=stdin,
              start_time=start_time)
        continue

      if code[vm.cp][0] == "TEST":
        old_cp = vm.cp
        item = vm.stack.pop()
        if item.tag != Tag.Bool:
          raise DlangRuntimeError
        if item.val:
          vm.cp += 1
        else:
          vm.cp = cp_from_label[code[vm.cp][1]]
        if old_cp < vm.cp:
          JIT_DRIVER.can_enter_jit(
              cp=vm.cp, verbosity=verbosity, code=code,
              cp_from_label=cp_from_label, vm=vm, stdin=stdin,
              start_time=start_time)
        continue

      if code[vm.cp][0] == "CASE":
        old_cp = vm.cp
        ptr = vm.stack.pop()
        if ptr.tag != Tag.HeapIndex:
          raise DlangRuntimeError
        item = ptr.ptr
        if item.tag == Tag.InlHeader:
          vm.stack.append(item.bdy[0])
          vm.cp += 1
        elif item.tag == Tag.InrHeader:
          vm.stack.append(item.bdy[0])
          vm.cp = cp_from_label[code[vm.cp][1]]
        else:
          raise DlangRuntimeError
        if old_cp < vm.cp:
          JIT_DRIVER.can_enter_jit(
              cp=vm.cp, verbosity=verbosity, code=code,
              cp_from_label=cp_from_label, vm=vm, stdin=stdin,
              start_time=start_time)
        continue

      raise Exception

    except DlangRuntimeError:
      if verbosity != "quiet":
        print "Runtime error at cp = " + str(vm.cp)
      return

  if verbosity != "output" and verbosity != "quiet":
    duration = str(int(1000000000 * time.time()) - start_time + 1000000000000)
    print (duration[1:4] + " s " +
           duration[4:7] + " ms " +
           duration[7:10] + " ms " +
           duration[10:13] + " us")

  if verbosity != "time" and verbosity != "quiet":
    print vm.print_result()


def entry_point(argv):
  """Entry point for the Python interpreter"""
  args = {}
  args[""] = ""
  i = 1
  while i < len(argv):
    if argv[i][:2] == "--":
      if i + 1 == len(argv):
        print "Missing argument for " + args[argv[i]]
        return 1
      args[argv[i]] = argv[i+1]
      i += 2
    else:
      args[""] = argv[i]
      i += 1

  filename = args[""]
  verbosity = args["--verbosity"] if "--verbosity" in args else "output"

  # Read the input code
  code = ""
  fp = os.open(filename, os.O_RDONLY, 0777)
  while True:
    read = os.read(fp, 4096)
    if not read:
      break
    code += read
  os.close(fp)

  # Run the interpreter
  interpret(code, verbosity)

  return 0

def target(*_):
  """Entry point for RPython"""
  return entry_point, None

# Main for Python
if __name__ == "__main__":
  entry_point(sys.argv)
