(* Copyright 2022 Federico Stazi. Subject to the MIT license. *)
(* Modified code from https://github.com/Timothy-G-Griffin/cc_cl_cam_ac_uk *)

(* If the file is "", prints bytecode to stdout *)
let print file code =
    match file with
    | "" -> print_string code
    | _  -> let f = open_out file in
            Printf.fprintf f "%s\n" code;
            close_out f
in
let front_end = Front_end.front_end Options.infile in
let bytecode = Compiler.compile front_end in
print Options.outfile (Compiler.string_of_listing bytecode)
