#!/usr/bin/python3

# Copyright 2022 Federico Stazi. Subject to the MIT license.

"""Command line interface to test dlang-vm"""

import json
import os
import random
import subprocess
import sys
import tempfile
from termcolor import colored


def get_commands():
  """Return a the list of commands for all interpreters and options"""
  # Options for dlang-vm
  jit_thresholds = ["0", "3"]
  jit_policies = ["no", "tracing", "individual", "block", "function"]
  memory_managers = ["none", "amortized", "mark-and-sweep"]
  optimizations = [
    "",
    "redundant-checks",
    "unused-writes",
    "copy-propagation",
    "dead-code",
    "constant-folding",
    "copy-propagation,dead-code",
    "copy-propagation,unused-writes",
    "copy-propagation,dead-code,redundant-checks",
    "redundant-checks,copy-propagation,dead-code",
    "copy-propagation,constant-folding",
    "copy-propagation,dead-code",
    "copy-propagation,unused-writes",
    "copy-propagation,constant-folding,dead-code,redundant-checks",
    "copy-propagation,dead-code,redundant-checks,constant-folding",
    "copy-propagation,dead-code,constant-folding,redundant-checks",
    "redundant-checks,copy-propagation,dead-code,constant-folding"
  ]

  # Construct the commands
  commands = []
  for jit_threshold in [("--jit-threshold", x) for x in jit_thresholds]:
    for jit_policy in [("--jit-policy", x) for x in jit_policies]:
      for memory_manager in [("--memory", x) for x in memory_managers]:
        for optimization in [("--optimizations", x) for x in optimizations]:
          commands.append(("../dlang_vm/dlang_vm",
                           jit_threshold + jit_policy +
                           memory_manager + optimization))
  commands.append(("../meta_dlang_vm.py", []))
  commands.append(("../meta_dlang_vm", []))

  return commands


def get_value(output):
  """Extract the output value from the program output"""
  if output:
    output = output.replace("input>", "")
    output = output.replace("output>", "")
    output = output.strip()
  return output


def fix_dharma(filename):
  """Specify variables (always in scope) in a dharma input"""
  output = []
  with open(filename, "r", encoding = "utf8") as program_file:
    scope = [0]
    waiting = []
    next_uuid = 1
    for token in program_file.read().split(" "):
      if token == "XDEF":
        output.append(f"x{next_uuid}")
        waiting.append(next_uuid)
        next_uuid += 1
      elif token == "XREF":
        idx = random.choice(scope)
        output.append(f"x{idx}")
      elif token == "ENTER_SCOPE":
        scope.append(waiting.pop())
      elif token == "EXIT_SCOPE":
        scope.pop()
      elif token == "SWAP_SCOPE":
        scope[-1], scope[-2] = scope[-2], scope[-1]
      else:
        output.append(token)

  with open(filename, "w", encoding = "utf8") as program_file:
    program_file.write(" ".join(output))


def compile_dlang(program, compiled):
  """Compile DLANG source code to bytecode"""
  try:
    subprocess.check_output(["../dlang_c/dlang_c", program, "-o", compiled],
                            stderr = subprocess.STDOUT,
                            text = True)

  except subprocess.CalledProcessError as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))
    print(ex.stdout)

  except Exception as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))


def run(program, compiled, *, options, verbosity, interpreter,
        program_input = None, print_out = False, timeout = None):
  """Run the program with the given options"""
  try:
    # Run the interpreter
    out = subprocess.check_output([interpreter, compiled,
                                   *options, "--verbosity", verbosity],
                                  input = program_input, text = True,
                                  stderr = subprocess.STDOUT, timeout = timeout)

    # Return and possibly print the output
    if print_out:
      print(out)
    return out

  except subprocess.TimeoutExpired as _:
    out = "TIMEOUT\n"
    if print_out:
      print(out)
    return out

  except subprocess.CalledProcessError as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))
    print(ex.stdout)

  except Exception as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))

  if print_out:
    print("None")
  return None


def run_slang(program, *, program_input = None):
  """Run the SLANG interpreter on the program"""
  try:
    # Run the SLANG interpreter
    out = subprocess.check_output(["../cc_cl_cam_ac_uk/slang/slang.byte",
                                   "-i4", program],
                                  input = program_input, text = True,
                                  stderr=subprocess.STDOUT)

    # Return the output
    return out

  except subprocess.CalledProcessError as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))
    print(ex.stdout)

  except Exception as ex:
    print(colored(f"Running {program} failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))

  return None


def run_unit_tests():
  """Run the unit tests of DlangVM"""
  try:
    subprocess.run(["../dlang_vm/build/tests/tests"],
                   stderr = subprocess.STDOUT, text = True)
  except FileNotFoundError:
    print(colored("Running unit tests failed, file not found, " +
                  "maybe you built dlang-vm in Release mode?", "red"))
  except Exception as ex:
    print(colored("Running unit tests failed, " +
                  f"{type(ex).__name__}: {ex}", "red"))


def run_differential_tests():
  """Run all tests in tests.json"""
  print("Running differential tests with interpreters:")
  print("\n".join([str(c) for c in get_commands()]))
  successes = 0
  failures = 0
  with open("tests.json", "r", encoding = "utf8") as tests_file:
    tests = json.load(tests_file)
    for i, test in enumerate(tests):
      print(f"*** input {i} out of {len(tests)} ***", end="\r")

      # If the expected output is not set in tests.json,
      # run the SLANG interpreter and assume its output is correct
      if test["output"]:
        expected_output = test["output"]
      else:
        expected_output = run_slang(test["file"], program_input = test["input"])

      with tempfile.NamedTemporaryFile() as compiled_tmp:
        # Compile the input
        compile_dlang(test["file"], compiled_tmp.name)

        # Run the test with all options
        for (interpreter, options) in get_commands():
          output = run(test["file"], compiled_tmp.name,
                      interpreter = interpreter,
                      options = options,
                      verbosity = "output",
                      program_input = test["input"],
                      print_out = False,
                      timeout = 10.0)

          # Keep track of successes and failures
          if get_value(output) == get_value(expected_output):
            successes += 1
          else:
            failures += 1
            print(colored(f"Failed test {test['file']} "
                          f"with interpreter {interpreter} "
                          f"and options {options}",
                          "yellow"))
            if expected_output:
              print(colored(f"Expected output: {get_value(expected_output)}",
                            "yellow"))
            else:
              print(colored("An error occurred when running "
                            "the SLANG interpreter, have you cloned and "
                            "built SLANG?", "yellow"))
            if output:
              print(colored("Interpreter output: "
                            f"{get_value(output)}", "yellow"))
            else:
              print(colored("An error occurred when running "
                            "the DLANG interpreter", "yellow"))

  # Print final report on the differential tests
  if failures:
    print(colored(f"Differential tests failed: {failures} failures "
                  f"out of {successes + failures} tests", "red"))
  else:
    print(colored(f"Differential tests succeeded "
                  f"({successes + failures} tests on "
                  f"{(successes + failures) // len(get_commands())} inputs)",
                  "green"))


def run_fuzz_tests():
  """Run fuzz tests"""
  print("Running fuzz tests with interpreters:")
  print("\n".join([str(c) for c in get_commands()]))
  num = 1000000
  successes = 0
  failures = 0
  groups_count = [0, 0, 0]
  with tempfile.TemporaryDirectory() as inputs_tmp:
    # Iterate over inputs divided into chunks of size k
    chunk_size = 1000
    for chunk_start in range(0, num * chunk_size, chunk_size):
      subprocess.check_call(["dharma", "-grammars", "dlang.dg",
                             "-logging", "20",
                             "-count", str(chunk_size),
                             "-seed", str(chunk_start // chunk_size),
                             "-storage", inputs_tmp])

      tests = [f"{inputs_tmp}/{i+1}.html" for i in range(chunk_size)]
      for i, test in enumerate(tests):
        print(f"*** input {i + chunk_start + 1} ***", end="\r")

        fix_dharma(test)

        with tempfile.NamedTemporaryFile() as compiled_tmp:
          # Compile the input
          compile_dlang(test, compiled_tmp.name)

          # Run the program on the first interpreter
          (interpreter0, options0) = get_commands()[0]
          statistics_output = run(test, compiled_tmp.name,
                                  interpreter = interpreter0,
                                  options = options0, verbosity = "statistics",
                                  print_out = False, timeout = 1.0)
          expected_output = run(test, compiled_tmp.name,
                                interpreter = interpreter0,
                                options = options0, verbosity = "output",
                                print_out = False, timeout = 1.0)

          # Stop if it does not terminate
          if expected_output[:7] == "TIMEOUT":
            continue

          # Get the number of interpreted instructions
          ex_instr_str = "Total number of interpreted instructions executed:"
          ex_instr = int(
              [x for x in statistics_output.split("\n") if ex_instr_str in x][0]
              .split(" ")[-1])

          # Get the total number of instructions in the program
          tot_instr = int(subprocess.check_output(["wc", "-l",
                                                   compiled_tmp.name],
                                                  stderr = subprocess.STDOUT,
                                                  text = True).split(" ")[0])

          # Get the group of this input
          group = 2
          if tot_instr < 200:
            group = 0
          if ex_instr > 20:
            group = 1

          # Skip if the program group does not satisfy the constraints
          if groups_count[group] > min(groups_count):
            continue
          groups_count[group] += 1

          # Run the test with all options
          for (interpreter, options) in get_commands():
            output = run(test, compiled_tmp.name,
                         interpreter = interpreter,
                         options = options,
                         verbosity = "output",
                         print_out = False,
                         timeout = 2.0)

            # Keep track of successes and failures
            if ((get_value(output) == get_value(expected_output)) or
                (get_value(output) == "TIMEOUT" and "none" not in options)):
              successes += 1
            else:
              failures += 1
              print(colored(f"Failed test {test} "
                            f"with interpreter {interpreter} "
                            f"and options {options}",
                            "yellow"))
              if expected_output:
                print(colored(f"Expected output: {get_value(expected_output)}",
                              "yellow"))
              else:
                print(colored("An error occurred when running "
                              "the SLANG interpreter", "yellow"))
              if output:
                print(colored("Interpreter output: "
                              f"{get_value(output)}", "yellow"))
              else:
                print(colored("An error occurred when running "
                              "the DLANG interpreter", "yellow"))

      print(f"\nTests: {sum(groups_count)}\t("
            f"Group 1: {groups_count[0]}, "
            f"Group 2: {groups_count[1]}, "
            f"Group 3: {groups_count[2]})")

    # Print final report on the fuzz tests
    if failures:
      print(colored(f"Fuzz tests failed: {failures} failures "
                    f"out of {successes + failures} tests", "red"))
    else:
      print(colored(f"Fuzz tests succeeded "
                    f"({successes + failures} tests on "
                    f"{(successes + failures) // len(get_commands())} inputs)",
                    "green"))


if __name__ == "__main__":
  random.seed(42)

  options = {
    "differential" : run_differential_tests,
    "unit"         : run_unit_tests,
    "fuzz"         : run_fuzz_tests
  }

  file_path = os.path.dirname(os.path.realpath(__file__))
  os.chdir(file_path)

  if len(sys.argv) != 2 or sys.argv[1] not in options:
    print ("Usage:" + "\n\t./test.py ".join([""] + list(options)))
  else:
    options[sys.argv[1]]()
