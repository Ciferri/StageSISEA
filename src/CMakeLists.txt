# General CMake configuration
################################################################################

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE OFF)
################################################################################
# General Project configuration
################################################################################

# project name
project(M2SLV01)

set(EXECUTABLE_OUTPUT_PATH
  ${CMAKE_BINARY_DIR}/bin
  CACHE PATH "Single output directory for building all executables."
  )

################################################################################
# Here go the add_subdirectories, no code should be at the root of the project
################################################################################

add_subdirectory(c++)

