(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

open Past

let complain = Errors.complain

let internal_error msg = complain ("INTERNAL ERROR: " ^ msg)

let rec find loc x = function
  | [] -> complain (x ^ " is not defined at " ^ (string_of_loc loc))
  | y :: rest -> if x = y then () else find loc x rest

let rec check_env env e =
    match e with
    | Unit _               -> e
    | What _               -> e
    | Integer _            -> e
    | Boolean _            -> e
    | Var (loc, x)         -> let _ = find loc x env in e
    | Seq(loc, el)         -> check_env_seq loc env el
    | While(loc, e1, e2)   -> While(loc, check_env env e1, check_env env e2)
    | Ref(loc, e)          -> Ref(loc, check_env env e)
    | Deref(loc, e)        -> Deref(loc, check_env env e)
    | Assign(loc, e1, e2)  -> Assign(loc, check_env env e1, check_env env e2)
    | UnaryOp(loc, uop, e) -> UnaryOp(loc, uop, check_env env e)
    | Op(loc, e1, bop, e2) -> Op(loc, check_env env e1, bop, check_env env e2)
    | If(loc, e1, e2, e3)  -> If(loc, check_env env e1, check_env env e2, check_env env e3)
    | Pair(loc, e1, e2)    -> Pair(loc, check_env env e1, check_env env e2)
    | Fst(loc, e)          -> Fst(loc, check_env env e)
    | Snd (loc, e)         -> Snd(loc, check_env env e)
    | Inl (loc, e)         -> Inl(loc, check_env env e)
    | Inr (loc, e)         -> Inr(loc, check_env env e)
    | Case(loc, e, (x1, e1), (x2, e2)) ->
            Case(loc, check_env env e, (x1, check_env (x1 :: env) e1), (x2, check_env (x2 :: env) e2))
    | Lambda (loc, (x, e)) -> Lambda(loc, (x, (check_env (x :: env) e)))
    | App(loc, e1, e2)     -> App(loc, check_env env e1, check_env env e2)
    | Let(loc, x, e1, e2)  -> Let(loc, x, check_env env e1, check_env (x :: env) e2)
    | LetFun(loc, f, (x, body), e) -> LetFun(loc, f, (x, check_env (f :: x :: env) body), check_env (f :: env) e)

and check_env_seq loc env el =
    let rec aux carry = function
      | []        -> internal_error "empty sequence found in parsed AST"
      | [e]       -> let e' = check_env env e in Seq(loc, List.rev (e' :: carry ))
      | e :: rest -> let e' = check_env env e in aux (e' :: carry) rest
    in aux [] el

let check e = check_env [] e
