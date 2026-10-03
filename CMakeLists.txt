cmake_minimum_required(VERSION 3.12)

set(CMAKE_POLICY_VERSION_MINIMUM 3.5)
project(phaselimiter C CXX)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
list(APPEND CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake)

option(BAKUAGE_USE_IPP "Use Intel IPP library" OFF)
option(SANDYBRIDGE_SUPPORT "support sandy bridge (x86 AVX without AVX2)" OFF)
option(DISABLE_TARGET_BENCH "disable bench" ON)
option(DISABLE_TARGET_TEST "disable test" OFF)
option(PHASELIMITER_STATIC_DEPS "Link static dependencies for portable distribution" OFF)
set(PHASELIMITER_DEPS_PREFIX "" CACHE PATH "Prefix for static dependencies")

if(PHASELIMITER_STATIC_DEPS)
  if(APPLE)
    if(NOT DEFINED CMAKE_OSX_DEPLOYMENT_TARGET OR CMAKE_OSX_DEPLOYMENT_TARGET STREQUAL "")
      set(CMAKE_OSX_DEPLOYMENT_TARGET "13.0" CACHE STRING "Minimum macOS deployment target" FORCE)
    endif()
  elseif(NOT WIN32)
    add_link_options(-static-libstdc++ -static-libgcc)
  endif()

  if(PHASELIMITER_DEPS_PREFIX)
    list(PREPEND CMAKE_PREFIX_PATH "${PHASELIMITER_DEPS_PREFIX}")
  elseif(DEFINED ENV{PHASELIMITER_DEPS_PREFIX})
    list(PREPEND CMAKE_PREFIX_PATH "$ENV{PHASELIMITER_DEPS_PREFIX}")
  endif()

  set(Boost_USE_STATIC_LIBS ON)
  set(Boost_USE_STATIC_RUNTIME OFF)
  set(BUILD_SHARED_LIBS OFF)
endif()

add_definitions(-DARMA_DONT_USE_HDF5)
add_definitions(-DARMA_DONT_USE_WRAPPER)
add_compile_definitions(cimg_display=0)
add_definitions(-DEIGEN_DONT_PARALLELIZE)
add_definitions(-DOPTIM_USE_TBB)
add_definitions(-Dcimg_use_png)

# Architecture-specific SIMD definitions
set(PHASELIMITER_TARGET_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
if(APPLE AND CMAKE_OSX_ARCHITECTURES)
  set(PHASELIMITER_TARGET_ARCH "${CMAKE_OSX_ARCHITECTURES}")
endif()

if(PHASELIMITER_TARGET_ARCH MATCHES "amd64|x86_64|AMD64")
  if(SANDYBRIDGE_SUPPORT)
    add_definitions(-DSIMDPP_ARCH_X86_AVX)
    if(NOT MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx")
    endif()
  else()
    add_definitions(-DSIMDPP_ARCH_X86_AVX)
    add_definitions(-DSIMDPP_ARCH_X86_AVX2)
    add_definitions(-DSIMDPP_ARCH_X86_FMA3)
    add_compile_definitions(BA_FMA_ENABLED)
    if(NOT MSVC)
      set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -mavx2 -mfma")
    endif()
  endif()
elseif(PHASELIMITER_TARGET_ARCH MATCHES "arm|aarch64|ARM64")
  add_definitions(-DSIMDPP_ARCH_ARM_NEON)
  add_definitions(-DSIMDPP_ARCH_ARM_NEON_FLT_SP)
endif()

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /MP /utf-8")
  if(CMAKE_SYSTEM_PROCESSOR MATCHES "amd64|x86_64|AMD64")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /arch:AVX")
  endif()
  set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /MD")
  set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /MDd")
  add_definitions(-DNOMINMAX)
elseif(APPLE)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -O3 -Wall -Wno-sign-compare -Wno-unused-variable -Wno-unused-but-set-variable")
endif()

# Auto-patch submodules for ARM and modern toolchains
set(_SIMDPP_SQRT_H "${CMAKE_CURRENT_SOURCE_DIR}/deps/libsimdpp/simdpp/detail/insn/f_sqrt.h")
if(EXISTS "${_SIMDPP_SQRT_H}")
  file(READ "${_SIMDPP_SQRT_H}" _SQRT_CONTENT)
  if(_SQRT_CONTENT MATCHES "SIMPDP_USE_NEON64")
    string(REPLACE "SIMPDP_USE_NEON64" "SIMDPP_USE_NEON64" _SQRT_CONTENT "${_SQRT_CONTENT}")
    file(WRITE "${_SIMDPP_SQRT_H}" "${_SQRT_CONTENT}")
    message(STATUS "Auto-patched libsimdpp typo in f_sqrt.h")
  endif()
endif()

set(_HNSW_SPACE_IP "${CMAKE_CURRENT_SOURCE_DIR}/deps/hnsw/hnswlib/space_ip.h")
if(EXISTS "${_HNSW_SPACE_IP}")
  file(READ "${_HNSW_SPACE_IP}" _IP_CONTENT)
  if(NOT _IP_CONTENT MATCHES "HNSW_NON_X86_GUARD")
    string(REPLACE "#include <x86intrin.h>" "// HNSW_NON_X86_GUARD\n#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)\n#include <x86intrin.h>\n#else\n#define InnerProductSIMD4Ext InnerProduct\n#define InnerProductSIMD16Ext InnerProduct\n#define InnerProductSIMD16ExtResiduals InnerProduct\n#define InnerProductSIMD4ExtResiduals InnerProduct\n#endif" _IP_CONTENT "${_IP_CONTENT}")
    string(REPLACE "static float\n    InnerProductSIMD4Ext" "#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)\n    static float\n    InnerProductSIMD4Ext" _IP_CONTENT "${_IP_CONTENT}")
    string(REPLACE "class InnerProductSpace :" "#endif\n\n    class InnerProductSpace :" _IP_CONTENT "${_IP_CONTENT}")
    file(WRITE "${_HNSW_SPACE_IP}" "${_IP_CONTENT}")
    message(STATUS "Auto-patched hnswlib space_ip.h for non-x86")
  endif()
endif()

set(_HNSW_SPACE_L2 "${CMAKE_CURRENT_SOURCE_DIR}/deps/hnsw/hnswlib/space_l2.h")
if(EXISTS "${_HNSW_SPACE_L2}")
  file(READ "${_HNSW_SPACE_L2}" _L2_CONTENT)
  if(NOT _L2_CONTENT MATCHES "HNSW_NON_X86_GUARD")
    string(REPLACE "#include <x86intrin.h>" "// HNSW_NON_X86_GUARD\n#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)\n#include <x86intrin.h>\n#else\n#define L2SqrSIMD16Ext L2Sqr\n#define L2SqrSIMD4Ext L2Sqr\n#define L2SqrSIMD16ExtResiduals L2Sqr\n#define L2SqrSIMD4ExtResiduals L2Sqr\n#endif" _L2_CONTENT "${_L2_CONTENT}")
    string(REPLACE "static float\n    L2SqrSIMD16Ext" "#if defined(__x86_64__) || defined(_M_X64) || defined(__i386__) || defined(_M_IX86)\n    static float\n    L2SqrSIMD16Ext" _L2_CONTENT "${_L2_CONTENT}")
    string(REPLACE "class L2Space :" "#endif\n\n    class L2Space :" _L2_CONTENT "${_L2_CONTENT}")
    file(WRITE "${_HNSW_SPACE_L2}" "${_L2_CONTENT}")
    message(STATUS "Auto-patched hnswlib space_l2.h for non-x86")
  endif()
endif()

set(_HNSW_HNSWLIB_H "${CMAKE_CURRENT_SOURCE_DIR}/deps/hnsw/hnswlib/hnswlib.h")
if(EXISTS "${_HNSW_HNSWLIB_H}")
  file(READ "${_HNSW_HNSWLIB_H}" _HNSW_CONTENT)
  if(NOT _HNSW_CONTENT MATCHES "_mm_prefetch")
    set(_HNSW_PREFETCH "#if !defined(__x86_64__) && !defined(_M_X64) && !defined(__i386__) && !defined(_M_IX86)\n#ifndef _mm_prefetch\n#define _mm_prefetch(p, h) ((void)0)\n#endif\n#endif\n")
    set(_HNSW_CONTENT "${_HNSW_PREFETCH}${_HNSW_CONTENT}")
    file(WRITE "${_HNSW_HNSWLIB_H}" "${_HNSW_CONTENT}")
    message(STATUS "Auto-patched hnswlib hnswlib.h for non-x86 prefetch")
  endif()
endif()

# Dependencies
find_package(Boost REQUIRED COMPONENTS filesystem serialization iostreams)
find_package(TBB REQUIRED)
if(TARGET TBB::tbbmalloc)
  set(TBBMALLOC_TARGET TBB::tbbmalloc)
else()
  find_library(TBBMALLOC_LIBRARY NAMES tbbmalloc REQUIRED)
  set(TBBMALLOC_TARGET ${TBBMALLOC_LIBRARY})
endif()
find_package(Armadillo QUIET)
find_package(PNG REQUIRED)
find_package(ZLIB REQUIRED)

if(PHASELIMITER_STATIC_DEPS)
  find_library(SNDFILE_LIBRARY NAMES libsndfile.a sndfile REQUIRED)
  find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h REQUIRED)
  set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
  set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
else()
  find_package(PkgConfig QUIET)
  if(PKG_CONFIG_FOUND)
    pkg_check_modules(SNDFILE REQUIRED sndfile)
  else()
    find_library(SNDFILE_LIBRARY NAMES sndfile libsndfile REQUIRED)
    find_path(SNDFILE_INCLUDE_DIR NAMES sndfile.h REQUIRED)
    set(SNDFILE_LIBRARIES ${SNDFILE_LIBRARY})
    set(SNDFILE_INCLUDE_DIRS ${SNDFILE_INCLUDE_DIR})
  endif()
endif()

if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/deps/optim/header_only_version/optim.hpp")
  message(STATUS "Generating OptimLib header-only version...")
  execute_process(
    COMMAND ./configure --header-only-version
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}/deps/optim
  )
endif()

set(BAKUAGE_LINK_LIBRARIES
  ${Boost_LIBRARIES}
  TBB::tbb
  ${TBBMALLOC_TARGET}
  ${SNDFILE_LIBRARIES}
  PNG::PNG
  ZLIB::ZLIB
  pthread
  m
)

if(ARMADILLO_FOUND)
  include_directories(${ARMADILLO_INCLUDE_DIRS})
  if(NOT PHASELIMITER_STATIC_DEPS)
    list(APPEND BAKUAGE_LINK_LIBRARIES ${ARMADILLO_LIBRARIES})
  endif()
endif()

if(WIN32)
  list(APPEND BAKUAGE_LINK_LIBRARIES shlwapi)
elseif(NOT APPLE)
  list(APPEND BAKUAGE_LINK_LIBRARIES dl)
endif()

if(BAKUAGE_USE_IPP)
  if(DEFINED ENV{IPPROOT})
    set(IPPROOT $ENV{IPPROOT})
  else()
    set(IPPROOT /opt/intel/ipp)
  endif()
  include_directories(${IPPROOT}/include)
  link_directories(${IPPROOT}/lib/intel64)
  list(APPEND BAKUAGE_LINK_LIBRARIES
    libippi.a
    libipps.a
    libippcore.a
    libippvm.a
  )
else()
  include_directories(deps/bakuage/include/ipp_compat)
endif()

if(NOT DISABLE_TARGET_TEST)
  set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  add_subdirectory(deps/googletest)
endif()

set(GFLAGS_BUILD_gflags_LIB ON CACHE BOOL "" FORCE)
set(GFLAGS_BUILD_gflags_nothreads_LIB OFF CACHE BOOL "" FORCE)
add_subdirectory(deps/gflags)

include_directories(
  src
  deps/bakuage/include
  deps/eigen
  deps/CImg
  deps/libsimdpp
  deps/hnsw
  deps/googletest/googletest/include
  deps/optim/header_only_version
  deps/picojson
  ${Boost_INCLUDE_DIRS}
  ${TBB_INCLUDE_DIRS}
)

if(SNDFILE_INCLUDE_DIRS)
  include_directories(${SNDFILE_INCLUDE_DIRS})
endif()

file(GLOB BAKUAGE_SOURCES deps/bakuage/src/*)
message(STATUS "BAKUAGE_SOURCES: ${BAKUAGE_SOURCES}")
message(STATUS "BAKUAGE_LINK_LIBRARIES: ${BAKUAGE_LINK_LIBRARIES}")

add_library(bakuage STATIC ${BAKUAGE_SOURCES})
target_link_libraries(bakuage ${BAKUAGE_LINK_LIBRARIES})

add_executable(build_test src/build_test/main.cpp)
target_link_libraries(build_test bakuage)

file(GLOB BAKUAGE_SND_FILE_INFO_SOURCES src/snd_file_info/*)
add_executable(snd_file_info ${BAKUAGE_SND_FILE_INFO_SOURCES})
target_link_libraries(snd_file_info bakuage gflags::gflags)

file(GLOB BAKUAGE_AUDIO_ANALYZER_SOURCES src/audio_analyzer/*)
add_executable(audio_analyzer ${BAKUAGE_AUDIO_ANALYZER_SOURCES})
target_link_libraries(audio_analyzer bakuage gflags::gflags)

file(GLOB BAKUAGE_PHASE_LIMITER_SOURCES src/phase_limiter/*)
add_executable(phase_limiter ${BAKUAGE_PHASE_LIMITER_SOURCES})
target_link_libraries(phase_limiter bakuage gflags::gflags)

file(GLOB BAKUAGE_SPECTROGRAM_SOURCES src/spectrogram/*)
add_executable(spectrogram ${BAKUAGE_SPECTROGRAM_SOURCES})
target_link_libraries(spectrogram bakuage gflags::gflags)

file(GLOB BAKUAGE_AUDIO_VISUALIZER_SOURCES src/audio_visualizer/*)
add_executable(audio_visualizer ${BAKUAGE_AUDIO_VISUALIZER_SOURCES})
target_link_libraries(audio_visualizer bakuage gflags::gflags)

file(GLOB BAKUAGE_CLEAR_MIXER_SOURCES src/clear_mixer/*)
add_executable(clear_mixer ${BAKUAGE_CLEAR_MIXER_SOURCES})
target_link_libraries(clear_mixer bakuage gflags::gflags)

file(GLOB BAKUAGE_EFFECT_TEST_SOURCES src/effect_test/*)
add_executable(effect_test ${BAKUAGE_EFFECT_TEST_SOURCES})
target_link_libraries(effect_test bakuage gflags::gflags)

if(NOT DISABLE_TARGET_TEST)
  file(GLOB BAKUAGE_TEST_SOURCES src/test/*)
  add_executable(test_bin ${BAKUAGE_TEST_SOURCES})
  target_link_libraries(test_bin bakuage gtest gflags::gflags)
endif()
