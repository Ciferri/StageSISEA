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
# Source code declarations
################################################################################

# m2sl basic header files



set(m2sl_HEADER_FILES
coupler.hpp
diffusion3D.hpp
Header_pgm.hpp
Image3D.hpp
prostTissue.hpp
model.hpp
prostateCell.hpp
prostCell.hpp
rootSimulator.hpp
simMeth.hpp
simulator.hpp
treatment.hpp
bmp.hpp
lodepng.hpp
MaFenetre.hpp
)

# m2sl basic source files
set(m2sl_SOURCE_FILES
coupler.cpp
diffusion3D.cpp
Header_pgm.cpp
Image3D.cpp
prostTissue.cpp
model.cpp
prostateCell.cpp
prostCell.cpp
rootSimulator.cpp
simMeth.cpp
simulator.cpp
treatment.cpp
bmp.cpp
lodepng.cpp
MaFenetre.cpp
)

set(TOOLS
main.cpp
)

# add_library(M2SLV01 SHARED   ${m2sl_SOURCE_FILES})
# 
# 
# install(TARGETS M2SL
#   RUNTIME DESTINATION bin COMPONENT libraries-core
#   LIBRARY DESTINATION lib COMPONENT libraries-core
#   ARCHIVE DESTINATION lib/static COMPONENT libraries-core
#   )
# 

find_package(Qt5Widgets)

# cmake_minimum_required(VERSION 2.8)
# 
# PROJECT(spline)
set (CMAKE_CXX_STANDARD 14)
# SET(SRCS_C Path3D_Splinetest.cpp path3d.cpp) #--- Les sources C++ ---
ADD_EXECUTABLE(m2slv01 ${m2sl_SOURCE_FILES} ${TOOLS})

target_link_libraries(m2slv01 Qt5::Widgets)
# SET(SRCS_C Path3D_SplineInterpolation.cpp path3d.cpp) #--- Les sources C++ ---
# ADD_EXECUTABLE(Path3D_SplineInterpolation ${SRCS_C})
# 
# SET(SRCS_C Path3D_ComputeSSDdistance.cpp path3d.cpp) #--- Les sources C++ ---
# ADD_EXECUTABLE(Path3D_ComputeSSDdistance ${SRCS_C})

# ADD_DEFINITIONS(-g -Wall -Wextra)


# Include and Link Directories
# INCLUDE_DIRECTORIES(${AT_INCLUDE_DIR} ${NII_DIR})
#INCLUDE_DIRECTORIES(${VTK_INCLUDE_DIRS} ${AT_INCLUDE_DIR} ${NII_DIR})
# LINK_DIRECTORIES(${AT_LIBRARY_DIR})
#INK_DIRECTORIES(${VTK_LIBRARY_DIRS} ${AT_LIBRARY_DIR})


# Executables and "stand-alone " librairies
# ADD_EXECUTABLE(DDC ${SRC} ${TOOLS} ${NII_SRC})

