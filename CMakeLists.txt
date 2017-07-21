# General CMake configuration
################################################################################

cmake_minimum_required(VERSION 2.8)
set(CMAKE_VERBOSE_MAKEFILE OFF)
################################################################################
# General Project configuration
################################################################################

# project name
project(M2SLV01)

################################################################################
# Here go the add_subdirectories, no code should be at the root of the project
################################################################################

add_subdirectory(src)
