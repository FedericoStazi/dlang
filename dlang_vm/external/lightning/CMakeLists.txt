# Copyright 2022 Federico Stazi. Subject to the MIT license.

ExternalProject_Add(
  # Download lightning from the official git repository
  liblightning
  GIT_REPOSITORY https://git.savannah.gnu.org/git/lightning.git
  GIT_SHALLOW TRUE
  GIT_PROGRESS TRUE
  UPDATE_DISCONNECTED TRUE

  # Run custom configuration commands for lightning
  CONFIGURE_COMMAND
    COMMAND cd <SOURCE_DIR> && ./bootstrap
    COMMAND <SOURCE_DIR>/configure CC=${CMAKE_C_COMPILER}
                                   CFLAGS=-Wno-format
                                   --prefix <SOURCE_DIR>/build
)

# Define variables for lightning
ExternalProject_Get_property(liblightning SOURCE_DIR)
set(Lightning_INCLUDE_DIRS ${SOURCE_DIR}/build/include)
set(Lightning_LIB_DIRS ${SOURCE_DIR}/build/lib)

# Include lighting header files
target_include_directories(dlang_vm PUBLIC ${Lightning_INCLUDE_DIRS})
if (TARGET tests)
  target_include_directories(tests PUBLIC ${Lightning_INCLUDE_DIRS})
endif()

# Create a CMake target from the installed shared library
add_library(lightning SHARED IMPORTED)
add_dependencies(lightning liblightning)
set_target_properties(lightning PROPERTIES
  IMPORTED_LOCATION ${Lightning_LIB_DIRS}/liblightning.so
)

# Link the library to the executable
target_link_libraries(dlang_vm lightning)
if (TARGET tests)
  target_link_libraries(tests lightning)
endif()
