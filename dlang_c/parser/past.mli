(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

(*
   The Parsed AST
*)

type var = string

type loc = Lexing.position

type oper = ADD | MUL | DIV | SUB | LT | AND | OR | EQ
type unary_oper = NEG | NOT

type expr =
       | Unit of loc
       | What of loc
       | Var of loc * var
       | Integer of loc * int
       | Boolean of loc * bool
       | UnaryOp of loc * unary_oper * expr
       | Op of loc * expr * oper * expr
       | If of loc * expr * expr * expr
       | Pair of loc * expr * expr
       | Fst of loc * expr
       | Snd of loc * expr
       | Inl of loc * expr
       | Inr of loc * expr
       | Case of loc * expr * lambda * lambda
       | While of loc * expr * expr
       | Seq of loc * (expr list)
       | Ref of loc * expr
       | Deref of loc * expr
       | Assign of loc * expr * expr
       | Lambda of loc * lambda
       | App of loc * expr * expr
       | Let of loc * var * expr * expr
       | LetFun of loc * var * lambda * expr
and lambda = var * expr

val loc_of_expr : expr -> loc
val string_of_loc : loc -> string

(* printing *)
val string_of_unary_oper : unary_oper -> string
val string_of_oper : oper -> string
val string_of_expr : expr -> string
val print_expr : expr -> unit
val eprint_expr : expr -> unit
