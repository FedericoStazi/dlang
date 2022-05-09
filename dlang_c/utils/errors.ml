(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

exception Error of string

let complain s = raise (Error s)
