(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

type code_index = int
type stack_index = int
type heap_index = int
type static_distance = int
type offset  = int

type label = string
type location = label * (code_index option)

type stack_item =
  | STACK_INT of int
  | STACK_BOOL of bool
  | STACK_UNIT
  | STACK_HI of heap_index    (* Heap Pointer   *)
  | STACK_RA of code_index    (* Return Address *)
  | STACK_FP of stack_index   (* Frame pointer  *)

type value_path =
  | STACK_LOCATION of offset
  | HEAP_LOCATION of offset

type instruction =
  | PUSH of stack_item
  | LOOKUP of value_path
  | UNARY of Ast.unary_oper
  | OPER of Ast.oper
  | ASSIGN
  | SWAP
  | POP
  | FST
  | SND
  | DEREF
  | APPLY
  | RETURN
  | MK_PAIR
  | MK_INL
  | MK_INR
  | MK_REF
  | MK_CLOSURE of location * int
  | TEST of location
  | CASE of location
  | GOTO of location
  | LABEL of label
  | FUNCTION of label
  | HALT


type listing = instruction list

val string_of_listing : listing -> string

val compile : Ast.expr -> listing
