cmake_minimum_required(VERSION 2.8)
project(DDMD)

# The build type "Release" adds some optimizations
if (NOT CMAKE_BUILD_TYPE)
  set (CMAKE_BUILD_TYPE "Release")
endif ()

# GCC on MacOs needs this option to use the clang assembler
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") AND (APPLE))
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -Wa,-q")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wa,-q")
  set (CMAKE_INCLUDE_SYSTEM_FLAG_C "-isystem")
  set (CMAKE_INCLUDE_SYSTEM_FLAG_CXX "-isystem")
endif ()

# Optimize to the current CPU and enable warnings
if ((CMAKE_CXX_COMPILER_ID STREQUAL "GNU") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "Clang") OR
    (CMAKE_CXX_COMPILER_ID STREQUAL "AppleClang"))
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -march=native -Wall -Wextra")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native -Wall -Wextra")
endif ()

# Enable C99
if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -std=gnu99")
else ()
  set (CMAKE_C_STANDARD 99)
endif ()

# Enable C++11
if (CMAKE_VERSION VERSION_LESS "3.1")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11")
else ()
  set (CMAKE_CXX_STANDARD 11)
endif ()

# Enable OpenMP
find_package (OpenMP)
set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")

# Link LibTIFF, LibJPEG, LibPNG
find_package (TIFF REQUIRED)
find_package (JPEG REQUIRED)
find_package (PNG REQUIRED)
include_directories (SYSTEM ${TIFF_INCLUDE_DIR} ${JPEG_INCLUDE_DIR} ${PNG_INCLUDE_DIRS})
link_libraries (${TIFF_LIBRARIES} ${JPEG_LIBRARIES} ${PNG_LIBRARIES})

# Link LibFFTW
find_path (FFTW_INCLUDE_DIR fftw3.h)
find_library (FFTWF_LIBRARIES NAMES fftw3f)
include_directories (PUBLIC ${FFTW_INCLUDE_DIR})
link_libraries (${FFTWF_LIBRARIES})
if (NOT FFTW_INCLUDE_DIR)
  message (FATAL_ERROR "FFTW3 not found.")
endif ()

# Enable Eigen
find_path (EIGEN3_INCLUDE_DIR NAMES signature_of_eigen3_matrix_library PATH_SUFFIXES eigen3 eigen)
include_directories (SYSTEM ${EIGEN3_INCLUDE_DIR})
if (NOT EIGEN3_INCLUDE_DIR)
  message (FATAL_ERROR "Eigen3 not found.")
endif ()

set_property(SOURCE nl-bayes/iio.c PROPERTY COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-parameter -Wno-pointer-sign -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-function")
set_property(SOURCE da3d/iio.c PROPERTY COMPILE_FLAGS "-Wno-unused-variable -Wno-unused-parameter -Wno-pointer-sign -Wno-parentheses -Wno-deprecated-declarations -Wno-unused-function")

set(NLB_FILES
  nl-bayes/main.cpp
  nl-bayes/NLBayes.cpp
  nl-bayes/NLBayes.h
  nl-bayes/iio.c
  nl-bayes/iio.h
  nl-bayes/Image.hpp
  nl-bayes/utils.cpp
  nl-bayes/utils.hpp)

set (DA3D_FILES
  da3d/DA3D.cpp
  da3d/DA3D.hpp
  da3d/DftPatch.hpp
  da3d/iio.c
  da3d/iio.h
  da3d/Image.hpp
  da3d/main.cpp
  da3d/Utils.cpp
  da3d/Utils.hpp
  da3d/WeightMap.cpp
  da3d/WeightMap.hpp)

set (DDMD_FILES
  nl-bayes/NLBayes.cpp
  nl-bayes/NLBayes.h
  nl-bayes/iio.c
  nl-bayes/iio.h
  nl-bayes/Image.hpp
  nl-bayes/utils.cpp
  nl-bayes/utils.hpp
  da3d/DA3D.cpp
  da3d/DA3D.hpp
  da3d/DftPatch.hpp
  da3d/Image.hpp
  da3d/Utils.cpp
  da3d/Utils.hpp
  da3d/WeightMap.cpp
  da3d/WeightMap.hpp
  DCT.cpp
  DCT.hpp
  da3d_ms.cpp)

add_executable(nl-bayes ${NLB_FILES})
add_executable(da3d ${DA3D_FILES})
add_executable(ddmd ${DDMD_FILES})
