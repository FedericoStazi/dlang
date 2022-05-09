(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

(*
   Parse command line options and args
*)
let infile         = ref ""
let outfile        = ref ""
let verbose_front  = ref false
let set_infile f   = infile := f

let option_spec = [
     ("-V",    Arg.Set verbose_front, "verbose front end");
     ("-o",    Arg.Set_string outfile, "compiled bytecode output file");
   ]
let usage_msg = "Usage: dlang.byte [options] [<file>]\nOptions are:"

let () = Arg.parse option_spec set_infile usage_msg

(* Set immutable versions of the options now that they have been parsed
 * Note: this is only to make the interface cleaner. *)
let infile        = !infile
let outfile       = !outfile
let verbose_front = !verbose_front
