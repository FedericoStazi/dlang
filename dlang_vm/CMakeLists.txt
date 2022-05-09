# Copyright 2022 Federico Stazi. Subject to the MIT license.

cmake_minimum_required(VERSION 3.18)
project(dlang_vm)
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_BUILD_TYPE Debug)

# Compile DLANG-VM
file(GLOB_RECURSE sources "src/*.cpp")
add_executable(dlang_vm ${sources})

# Compile unit tests (if in debug mode)
if (CMAKE_BUILD_TYPE STREQUAL "Debug")
  add_subdirectory(tests)
endif()

# Add and link external libraries
add_subdirectory(external)
