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

let  loc_of_expr = function
    | Unit loc                      -> loc
    | What loc                      -> loc
    | Var (loc, _)                  -> loc
    | Integer (loc, _)              -> loc
    | Boolean (loc, _)              -> loc
    | UnaryOp(loc, _, _)            -> loc
    | Op(loc, _, _, _)              -> loc
    | If(loc, _, _, _)              -> loc
    | Pair(loc, _, _)               -> loc
    | Fst(loc, _)                   -> loc
    | Snd(loc, _)                   -> loc
    | Inr(loc, _)                -> loc
    | Inl(loc, _)                -> loc
    | Case(loc, _, _, _)            -> loc
    | Seq(loc, _)                   -> loc
    | Ref(loc, _)                   -> loc
    | Deref(loc, _)                 -> loc
    | Assign(loc, _, _)             -> loc
    | While(loc, _, _)              -> loc
    | Lambda(loc, _)                -> loc
    | App(loc, _, _)                -> loc
    | Let(loc, _, _, _)          -> loc
    | LetFun(loc, _, _, _)       -> loc


let string_of_loc loc =
    "line " ^ (string_of_int (loc.Lexing.pos_lnum)) ^ ", " ^
    "position " ^ (string_of_int ((loc.Lexing.pos_cnum - loc.Lexing.pos_bol) + 1))

open Format

(*
   Documentation of Format can be found here:
   http://caml.inria.fr/resources/doc/guides/format.en.html
   http://caml.inria.fr/pub/docs/manual-ocaml/libref/Format.html
*)

let pp_uop = function
  | NEG -> "-"
  | NOT -> "~"


let pp_bop = function
  | ADD -> "+"
  | MUL  -> "*"
  | DIV  -> "/"
  | SUB -> "-"
  | LT   -> "<"
  | EQ   -> "="
  | AND   -> "&&"
  | OR   -> "||"

let string_of_oper = pp_bop
let string_of_unary_oper = pp_uop

let fstring ppf s = fprintf ppf "%s" s
let pp_unary ppf op = fstring ppf (pp_uop op)
let pp_binary ppf op = fstring ppf (pp_bop op)

(* ignore locations *)
let rec pp_expr ppf = function
    | Unit _              -> fstring ppf "()"
    | What _              -> fstring ppf "?"
    | Var (_, x)          -> fstring ppf x
    | Integer (_, n)      -> fstring ppf (string_of_int n)
    | Boolean (_, b)      -> fstring ppf (string_of_bool b)
    | UnaryOp(_, op, e)   -> fprintf ppf "%a(%a)" pp_unary op pp_expr e
    | Op(_, e1, op, e2)   -> fprintf ppf "(%a %a %a)" pp_expr e1  pp_binary op pp_expr e2
    | If(_, e1, e2, e3)   -> fprintf ppf "@[if %a then %a else %a @]"
                                      pp_expr e1 pp_expr e2 pp_expr e3
    | Pair(_, e1, e2)     -> fprintf ppf "(%a, %a)" pp_expr e1 pp_expr e2
    | Fst(_, e)           -> fprintf ppf "fst(%a)" pp_expr e
    | Snd(_, e)           -> fprintf ppf "snd %a" pp_expr e
    | Inl(_, e)        -> fprintf ppf "(inl %a)" pp_expr e
    | Inr(_, e)        -> fprintf ppf "(inr %a)" pp_expr e
    | Case(_, e, (x1, e1), (x2, e2)) ->
        fprintf ppf "@[<2>case %a of@ | inl(%a) -> %a @ | inr(%a) -> %a end@]"
                     pp_expr e fstring x1 pp_expr e1 fstring x2 pp_expr e2

    | Seq (_, [])         -> ()
    | Seq (_, [e])        -> pp_expr ppf e
    | Seq (l, e :: rest)  -> fprintf ppf "%a; %a" pp_expr e pp_expr (Seq(l, rest))
    | While (_, e1, e2)   -> fprintf ppf "while %a do %a end" pp_expr e1 pp_expr e2
    | Ref(_, e)           -> fprintf ppf "ref %a" pp_expr e
    | Deref(_, e)         -> fprintf ppf "!%a" pp_expr e
    | Assign(_, e1, e2)   -> fprintf ppf "(%a := %a)" pp_expr e1 pp_expr e2
    | Lambda(_, (x, e)) ->
         fprintf ppf "(fun %a -> %a)" fstring x pp_expr e
    | App(_, e1, e2)      -> fprintf ppf "%a %a" pp_expr e1 pp_expr e2
    | Let(_, x, e1, e2) ->
         fprintf ppf "@[<2>let %a = %a in %a end@]" fstring x pp_expr e1 pp_expr e2
    | LetFun(_, f, (x, e1), e2)     ->
         fprintf ppf "@[let %a(%a) =@ %a @ in %a @ end@]"
                     fstring f fstring x pp_expr e1 pp_expr e2

let print_expr e =
    let _ = pp_expr std_formatter e
    in print_flush ()

let eprint_expr e =
    let _ = pp_expr err_formatter e
    in print_flush ()

(* useful for degugging *)


let string_of_uop = function
  | NEG -> "NEG"
  | NOT -> "NOT"

let string_of_bop = function
  | ADD -> "ADD"
  | MUL  -> "MUL"
  | DIV  -> "DIV"
  | SUB -> "SUB"
  | LT   -> "LT"
  | EQ   -> "EQ"
  | AND   -> "AND"
  | OR   -> "OR"

let mk_con con l =
    let rec aux carry = function
      | [] -> carry ^ ")"
      | [s] -> carry ^ s ^ ")"
      | s::rest -> aux (carry ^ s ^ ", ") rest
    in aux (con ^ "(") l

let rec string_of_expr = function
    | Unit _              -> "Unit"
    | What _              -> "What"
    | Var (_, x)          -> mk_con "Var" [x]
    | Integer (_, n)      -> mk_con "Integer" [string_of_int n]
    | Boolean (_, b)      -> mk_con "Boolean" [string_of_bool b]
    | UnaryOp(_, op, e)   -> mk_con "UnaryOp" [string_of_uop op; string_of_expr e]
    | Op(_, e1, op, e2)   -> mk_con "Op" [string_of_expr e1; string_of_bop op; string_of_expr e2]
    | If(_, e1, e2, e3)   -> mk_con "If" [string_of_expr e1; string_of_expr e2; string_of_expr e3]
    | Pair(_, e1, e2)     -> mk_con "Pair" [string_of_expr e1; string_of_expr e2]
    | Fst(_, e)           -> mk_con "Fst" [string_of_expr e]
    | Snd(_, e)           -> mk_con "Snd" [string_of_expr e]
    | Inl(_, e)        -> mk_con "Inl" [string_of_expr e]
    | Inr(_, e)        -> mk_con "Inr" [string_of_expr e]
    | Seq (_, el)         -> mk_con "Seq" [string_of_expr_list el]
    | While (_, e1, e2)   -> mk_con "While" [string_of_expr e1; string_of_expr e2]
    | Ref(_, e)           -> mk_con "Ref" [string_of_expr e]
    | Deref(_, e)         -> mk_con "Deref" [string_of_expr e]
    | Assign(_, e1, e2)   -> mk_con "Assign" [string_of_expr e1; string_of_expr e2]
    | Lambda(_, (x, e)) -> mk_con "Lambda" [x; string_of_expr e]
    | App(_, e1, e2)      -> mk_con "App" [string_of_expr e1; string_of_expr e2]
    | Let(_, x, e1, e2) -> mk_con "Let" [x; string_of_expr e1; string_of_expr e2]
    | LetFun(_, f, (x, e1), e2)      ->
          mk_con "LetFun" [
             f; mk_con "" [x; string_of_expr e1]; string_of_expr e2]
    | Case(_, e, (x1, e1), (x2, e2)) ->
          mk_con "Case" [
	     string_of_expr e;
	     mk_con "" [x1; string_of_expr e1];
	     mk_con "" [x2; string_of_expr e2]]

and string_of_expr_list = function
  | [] -> ""
  | [e] -> string_of_expr e
  |  e:: rest -> (string_of_expr e ) ^ "; " ^ (string_of_expr_list rest)
