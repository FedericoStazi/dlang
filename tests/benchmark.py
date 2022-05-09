#!/usr/bin/python3

# Copyright 2022 Federico Stazi. Subject to the MIT license.

"""Test the performance of multiple DLANG interpreters on some programs"""

import os
import subprocess
import math


def compile_dlang(program, compiled):
  """Compile the source code to bytecode"""
  subprocess.check_output(["../dlang_c/dlang_c", program, "-o", compiled],
                          stderr = subprocess.STDOUT, text = True)


def benchmark(command, cpu = 1, timeout = 120):
  """Run the command on the given core with a certain timeout"""
  command_full = (f"taskset -c {cpu} "
                  f"nice -n -5 "
                  f"/usr/bin/time -f %E,%M "
                  f"timeout {timeout} "
                  + command)

  output = subprocess.run(command_full.split(" "),
                          stderr = subprocess.PIPE,
                          stdout = subprocess.PIPE,
                          text = True).stderr

  time, space = output.split("\n")[-2].split(",")
  time = 10 * int(100.0 * (float(time.split(":")[0]) * 60.0 +
                          float(time.split(":")[1])))
  space = 10 * int(space)
  return time, space


def evaluate(idx, results, z_confidence):
  """Print the mean and confidence interval for the results"""
  results_cnt = len(results)
  results_mean = sum(results) / results_cnt
  results_squared_sum = 0 if results_cnt < 2 else (
    sum([(ri - results_mean)**2 for ri in results]) / (results_cnt - 1))
  confidence_range = (
    z_confidence * math.sqrt(results_squared_sum) / math.sqrt(results_cnt))
  print(f"id: {idx} {results_mean:.3f}ms±{confidence_range:.3f}ms")


def make_commands():
  """Generate the commands to be tested"""
  opt = ("--optimizations "
         "redundant-checks,copy-propagation,constant-folding,dead-code")
  verb = "--verbosity quiet"
  mem = "--memory amortized"
  interpreters = {
      "function":     f"../dlang_vm/dlang_vm "
                        f"--jit-policy function {mem} {verb}",
      "tracing":      f"../dlang_vm/dlang_vm "
                        f"--jit-policy tracing {mem} {verb}",
      "meta-tracing": f"../meta_dlang_vm {verb}",
      "function-opt": f"../dlang_vm/dlang_vm "
                        f"--jit-policy function {opt} {mem} {verb}",
      "tracing-opt":  f"../dlang_vm/dlang_vm "
                        f"--jit-policy tracing {opt} {mem} {verb}",
  }
  tests = ["fib", "ack", "prim", "queen", "sort", "hanoi"]

  for test in tests:
    compile_dlang(f"inputs/{test}.dlang", f"{test}.out")

  commands = {}
  for interpreter_id, interpreter in interpreters.items():
    for test in tests:
      commands[f"{interpreter_id}-{test}"] = f"{interpreter} {test}.out"

  return commands


if __name__ == "__main__":
  Z_CONFIDENCE = 1.96
  NUM_TESTS = 1000

  file_path = os.path.dirname(os.path.realpath(__file__))
  os.chdir(file_path)

  commands = make_commands()

  results = {i:[] for i in commands}

  for test in range(NUM_TESTS):
    print(f"{test} out of {NUM_TESTS}")
    for command_id, command in commands.items():
      t, _ = benchmark(command)
      results[command_id].append(t)
      evaluate(command_id, results[command_id], Z_CONFIDENCE)
