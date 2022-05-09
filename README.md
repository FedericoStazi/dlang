# dlang
Program execution tools for a minimalistic dynamically typed language.

<h2>Table of Contents</h2>

- [Clone](#Clone)
- [Build](#Build)
- [Run](#Run)
  - [DLANG-C](#DLANG-C)
  - [DLANG-VM](#DLANG-VM)
  - [Meta-DLANG-VM](#Meta-DLANG-VM)
- [Test](#Test)
  - [Unit](#Unit)
  - [Differential](#Differential)
  - [Fuzz](#Fuzz)
- [Benchmark](#Benchmark)

## Clone

Git clone the repository (using `--recursive` to also include Sub-Modules):
```
git clone --recursive https://github.com/FedericoStazi/dlang.git
```

Enter the `dlang/` folder:
```
cd dlang/
```

## Build

Build Slang, DLANG-C and DLANG-VM:
```
./build.py all
```

Build Meta-DLANG-VM:
```
./build.py meta
```

## Run

#### DLANG-C

Compile a program `program.dlang` to byte-code:
```
./dlang_c/dlang_c program.dlang -o program.out
```

#### DLANG-VM

Interpret a byte-code program `program.out` using DLANG-VM:
```
./dlang_vm/dlang_vm program.out
```

DLANG-VM provides many arguments, such as:
```
./dlang_vm/dlang_vm --jit-policy function --memory mark-and-sweep --optimizations copy-propagation,redundant-checks program.out
```

#### Meta-DLANG-VM

Interpret a byte-code program `program.out` using Meta-DLANG-VM:
```
./meta_dlang_vm program.out
```

## Test

#### Unit

Run unit tests (only available if building DLANG-VM in `Debug` mode):
```
./tests/test.py unit
```

#### Differential

Run differential tests:
```
./tests/test.py differential
```

#### Fuzz

Run fuzz tests:
```
./tests/test.py fuzz
```

## Benchmark

Run benchmarks:
```
./tests/benchmark.py
```
