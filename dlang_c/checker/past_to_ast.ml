(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

(*   translate_expr : Past.expr -> Ast.expr
     Translates parsed AST to internal AST :
     1) drop file locations
     2) remove let
     3) replace "?" (What) with unary function call (Read)
*)

let translate_uop = function
  | Past.NEG -> Ast.NEG
  | Past.NOT -> Ast.NOT

let translate_bop = function
  | Past.ADD -> Ast.ADD
  | Past.MUL -> Ast.MUL
  | Past.DIV -> Ast.DIV
  | Past.SUB -> Ast.SUB
  | Past.LT  -> Ast.LT
  | Past.AND -> Ast.AND
  | Past.OR  -> Ast.OR
  | Past.EQ  -> Ast.EQ

let rec translate_expr = function
    | Past.Unit _                -> Ast.Unit
    | Past.What _                -> Ast.UnaryOp(Ast.READ, Ast.Unit)
    | Past.Var(_, x)             -> Ast.Var x
    | Past.Integer(_, n)         -> Ast.Integer n
    | Past.Boolean(_, b)         -> Ast.Boolean b
    | Past.UnaryOp(_, op, e)     -> Ast.UnaryOp(translate_uop op, translate_expr e)
    | Past.Op(_, e1, op, e2)     -> Ast.Op(translate_expr e1, translate_bop op, translate_expr e2)
    | Past.If(_, e1, e2, e3)     -> Ast.If(translate_expr e1, translate_expr e2, translate_expr e3)
    | Past.Pair(_, e1, e2)       -> Ast.Pair(translate_expr e1, translate_expr e2)
    | Past.Fst(_, e)             -> Ast.Fst(translate_expr e)
    | Past.Snd(_, e)             -> Ast.Snd(translate_expr e)
    | Past.Inl(_, e)             -> Ast.Inl(translate_expr e)
    | Past.Inr(_, e)             -> Ast.Inr(translate_expr e)
    | Past.Case(_, e, l1, l2)    ->
         Ast.Case(translate_expr e, translate_lambda l1, translate_lambda l2)
    | Past.Lambda(_, l)          -> Ast.Lambda (translate_lambda l)
    | Past.App(_, e1, e2)        -> Ast.App(translate_expr e1, translate_expr e2)
    | Past.Let(_, x, e1, e2)     ->
         Ast.App(Ast.Lambda(x, translate_expr e2), translate_expr e1)
    | Past.LetFun(_, f, l, e)    ->
         Ast.LetFun(f, translate_lambda l, translate_expr e)
    | Past.Seq(_, el)            -> Ast.Seq(List.map translate_expr el)
    | Past.While(_, e1, e2)      -> Ast.While(translate_expr e1, translate_expr e2)
    | Past.Ref(_, e)             -> Ast.Ref(translate_expr e)
    | Past.Deref(_, e)           -> Ast.Deref(translate_expr e)
    | Past.Assign(_, e1, e2)     -> Ast.Assign(translate_expr e1, translate_expr e2)
and translate_lambda (x, body) = (x, translate_expr body)
