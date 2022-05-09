(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

open Ast

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
  | UNARY of unary_oper
  | OPER of oper
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

(********************** Printing ********************************)

let string_of_list sep f l =
   let rec aux f = function
     | [] -> ""
     | [t] -> (f t)
     | t :: rest -> (f t) ^  sep  ^ (aux f rest)
   in "[" ^ (aux f l) ^ "]"

let string_of_stack_item = function
  | STACK_INT i      -> "STACK_INT " ^ (string_of_int i)
  | STACK_BOOL true  -> "STACK_BOOL true"
  | STACK_BOOL false -> "STACK_BOOL false"
  | STACK_UNIT       -> "STACK_UNIT"
  | STACK_HI i       -> "STACK_HI " ^ (string_of_int i)
  | STACK_RA i       -> "STACK_RA " ^ (string_of_int i)
  | STACK_FP i       -> "STACK_FP " ^ (string_of_int i)

let string_of_value_path = function
  | STACK_LOCATION offset -> "STACK_LOCATION " ^ (string_of_int offset)
  | HEAP_LOCATION offset  -> "HEAP_LOCATION " ^ (string_of_int offset)

let string_of_location = function
  | (l, None) -> l
  | (l, Some i) -> l ^ " = " ^ (string_of_int i)

let string_of_instruction = function
 | UNARY op -> "UNARY " ^ (string_of_uop op)
 | OPER op  -> "OPER " ^ (string_of_bop op)
 | MK_PAIR  -> "MK_PAIR"
 | FST      -> "FST"
 | SND      -> "SND"
 | MK_INL   -> "MK_INL"
 | MK_INR   -> "MK_INR"
 | MK_REF   -> "MK_REF"
 | PUSH v   -> "PUSH " ^ (string_of_stack_item v)
 | LOOKUP p -> "LOOKUP " ^ (string_of_value_path p)
 | TEST l   -> "TEST " ^ (string_of_location l)
 | CASE l   -> "CASE " ^ (string_of_location l)
 | GOTO l   -> "GOTO " ^ (string_of_location l)
 | APPLY    -> "APPLY"
 | RETURN   -> "RETURN"
 | HALT     -> "HALT"
 | LABEL l  -> "LABEL " ^ l
 | FUNCTION l -> "FUNCTION " ^ l
 | SWAP     -> "SWAP"
 | POP      -> "POP"
 | DEREF    -> "DEREF"
 | ASSIGN   -> "ASSIGN"
 | MK_CLOSURE (loc, n)
             -> "MK_CLOSURE " ^ (string_of_location loc)
	                      ^ " " ^ (string_of_int n)

let rec string_of_listing = function
  | [] -> ""
  | i :: rest -> (string_of_instruction i) ^ "\n" ^ (string_of_listing rest)

let rec find l y =
  match l with
  | [] -> Errors.complain ("Compile.find : " ^ y ^ " is not found")
  | (x, v) :: rest -> if x = y then v else find rest y

(* COMPILE *)

let new_label =
    let i = ref 0 in
    let get () = let v = !i in (i := (!i) + 1; "L" ^ (string_of_int v))
    in get

let positions l =
    let rec aux k = function
    | [] -> []
    | a :: rest -> (a, k) :: (aux (k+1) rest)
    in aux 1 l

let rec comp vmap = function
  | Unit           -> ([], [PUSH STACK_UNIT])
  | Boolean b      -> ([], [PUSH (STACK_BOOL b)])
  | Integer n      -> ([], [PUSH (STACK_INT n)])
  | UnaryOp(op, e) -> let (defs, c) = comp vmap e in  (defs, c @ [UNARY op])
  | Op(e1, op, e2) -> let (defs1, c1) = comp vmap e1 in
                      let (defs2, c2) = comp vmap e2 in
                          (defs1 @ defs2, c1 @ c2 @ [OPER op])
  | Pair(e1, e2)   -> let (defs1, c1) = comp vmap e1 in
                      let (defs2, c2) = comp vmap e2 in
                          (defs1 @ defs2, c1 @ c2 @ [MK_PAIR])
  | Fst e          -> let (defs, c) = comp vmap e in (defs, c @ [FST])
  | Snd e          -> let (defs, c) = comp vmap e in (defs, c @ [SND])
  | Inl e          -> let (defs, c) = comp vmap e in (defs, c @ [MK_INL])
  | Inr e          -> let (defs, c) = comp vmap e in (defs, c @ [MK_INR])
  | Case(e1, (x1, e2), (x2, e3)) ->
                      let inr_label = new_label () in
                      let after_inr_label = new_label () in
                      let (defs1, c1) = comp vmap e1 in
                      let (defs2, c2) = comp vmap (Lambda(x1, e2)) in
                      let (defs3, c3) = comp vmap (Lambda(x2, e3)) in
                         (defs1 @ defs2 @ defs3,
                          (c1
   		           @ [CASE(inr_label, None)]
                           @ c2
		           @ [APPLY; GOTO (after_inr_label, None);
                              LABEL inr_label]
                           @ c3
		           @ [APPLY; LABEL after_inr_label]))
  | If(e1, e2, e3) -> let else_label = new_label () in
                      let after_else_label = new_label () in
                      let (defs1, c1) = comp vmap e1 in
                      let (defs2, c2) = comp vmap e2 in
                      let (defs3, c3) = comp vmap e3 in
                         (defs1 @ defs2 @ defs3,
                          (c1
   		           @ [TEST(else_label, None)]
                           @ c2
		           @ [GOTO (after_else_label, None); LABEL else_label]
                           @ c3
		           @ [LABEL after_else_label]))
 | Seq []         -> ([], [])
 | Seq [e]        -> comp vmap e
 | Seq (e ::rest) -> let (defs1, c1) = comp vmap e in
                     let (defs2, c2) = comp vmap (Seq rest) in
                       (defs1 @ defs2, c1 @ [POP] @ c2)
 | Ref e          -> let (defs, c) = comp vmap e in (defs, c @ [MK_REF])
 | Deref e        -> let (defs, c) = comp vmap e in (defs, c @ [DEREF])
 | While(e1, e2)  -> let test_label = new_label () in
                      let end_label = new_label () in
                      let (defs1, c1) = comp vmap e1 in
                      let (defs2, c2) = comp vmap e2 in
                         (defs1 @ defs2,
                          [LABEL test_label]
                           @ c1
                           @ [TEST(end_label, None)]
                           @ c2
                           @ [POP; GOTO (test_label, None); LABEL end_label; PUSH STACK_UNIT])
 | Assign(e1, e2) -> let (defs1, c1) = comp vmap e1 in
                     let (defs2, c2) = comp vmap e2 in
                         (defs1 @ defs2, c1 @ c2 @ [ASSIGN])

 | App(e1, e2)    -> let (defs1, c1) = comp vmap e1 in
                     let (defs2, c2) = comp vmap e2 in
                          (defs1 @ defs2, c2 @ c1 @ [APPLY])
 | Var x           -> ([], [LOOKUP(find vmap x)])
 | Lambda(x, e)           -> comp_lambda vmap (None, x, e)
 | LetFun(f, (x, e1), e2) ->
                      let (defs1, c1) = comp vmap (Lambda(f, e2)) in
                      let (defs2, c2) = comp_lambda vmap (Some f, x, e1) in
                          (defs1 @ defs2, c2 @ c1 @ [APPLY])

and comp_lambda vmap (f_opt, x, e) =
    let bound_vars = match f_opt with | None -> [x]          | Some f -> [x; f] in
    let f = new_label () in
    let f_bind =     match f_opt with | None -> []           | Some f -> [(f, STACK_LOCATION (-1))]  in
    let x_bind = (x, STACK_LOCATION (-2)) in
    let fvars = Free_vars.free_vars (bound_vars, e)   in
    let fetch_fvars = List.map (fun y -> LOOKUP(find vmap y)) fvars in
    let fvar_bind (y, p) = (y, HEAP_LOCATION p) in
    let env_bind = List.map fvar_bind (positions fvars) in
    let new_vmap = x_bind :: (f_bind @ env_bind @ vmap) in
    let (defs, c) = comp new_vmap e in
    let def = [FUNCTION f] @ c @ [RETURN] in
     (def @ defs, (List.rev fetch_fvars) @ [MK_CLOSURE((f, None), List.length fvars)])

let compile e =
    let (defs, c) = comp [] e in
        c          (* body of program *)
        @ [HALT]   (* stop the interpreter *)
        @ defs     (* the function definitions *)
