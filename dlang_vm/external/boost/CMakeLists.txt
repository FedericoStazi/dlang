# Copyright 2022 Federico Stazi. Subject to the MIT license.

# Find, include and link boost library for program options
find_package(Boost 1.65 COMPONENTS program_options REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

target_link_libraries(dlang_vm ${Boost_LIBRARIES})

if (TARGET tests)
  target_link_libraries(tests ${Boost_LIBRARIES})
endif()
