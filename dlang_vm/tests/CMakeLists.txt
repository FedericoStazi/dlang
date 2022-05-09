# Copyright 2022 Federico Stazi. Subject to the MIT license.

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
FetchContent_MakeAvailable(googletest)

enable_testing()

file(GLOB_RECURSE test_sources "*.cpp")
file(GLOB_RECURSE dlang_vm_sources "../src/*.cpp")
list(FILTER dlang_vm_sources EXCLUDE REGEX ".*main.cpp")
add_executable(tests ${test_sources} ${dlang_vm_sources})
target_link_libraries(tests gtest_main)

include(GoogleTest)
gtest_discover_tests(tests)
