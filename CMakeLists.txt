cmake_minimum_required(VERSION 3.25)


project(SimdPlayground VERSION 1.0
                       DESCRIPTION "A collection of utilities for working with std::experimental::simd"
                       LANGUAGES CXX)

enable_testing()

include(FetchContent)
include(CMakePrintHelpers)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED YES)
set(CMAKE_CXX_EXTENSIONS NO)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_BUILD_TYPE Release)

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-march=native" COMPILER_SUPPORTS_MARCH_NATIVE)
if(COMPILER_SUPPORTS_MARCH_NATIVE)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")
endif()

# Necessary to use libstdc++ for std::simd
# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libstdc++")

# Externally provided libraries
FetchContent_Declare(googletest
                     GIT_REPOSITORY https://github.com/google/googletest.git
                     GIT_TAG v1.10.x)

FetchContent_Declare(googlebenchmark
                     GIT_REPOSITORY https://github.com/google/benchmark.git
                     GIT_TAG main) # need main for benchmark::benchmark

option(INSTALL_GTEST OFF)
option(INSTALL_BENCHMARK OFF)
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest googlebenchmark)
# set(BENCHMARK_ENABLE_TESTING OFF)
# set(BENCHMARK_ENABLE_GTEST_TESTS OFF)

include(GoogleTest)

add_library(common INTERFACE)
target_include_directories(common INTERFACE src/lib/common/include)
target_compile_features(common INTERFACE cxx_std_20)

add_library(simdalgorithm INTERFACE)
target_include_directories(simdalgorithm INTERFACE src/lib/simdalgorithm/include)
target_link_libraries(simdalgorithm INTERFACE common)
target_compile_features(simdalgorithm INTERFACE cxx_std_20)

add_library(simdviews INTERFACE)
target_include_directories(simdviews INTERFACE src/lib/simdviews/include)
target_link_libraries(simdviews INTERFACE common)
target_compile_features(simdviews INTERFACE cxx_std_20)

add_library(staticbtree INTERFACE)
target_include_directories(staticbtree INTERFACE src/lib/staticbtree/include)
target_link_libraries(staticbtree INTERFACE common)
target_compile_features(staticbtree INTERFACE cxx_std_20)

# Tests

add_executable(staticbtree_test src/lib/staticbtree/test/staticbtree_test.cpp)
target_link_libraries(staticbtree_test PUBLIC staticbtree gtest_main)
target_compile_features(staticbtree_test PUBLIC cxx_std_20)
gtest_discover_tests(staticbtree_test)

# Benchmarks

add_executable(lower_bound_benchmark src/benchmarks/lower_bound_benchmark.cpp)
target_link_libraries(lower_bound_benchmark PUBLIC simdviews staticbtree benchmark::benchmark)
target_compile_features(lower_bound_benchmark PUBLIC cxx_std_20)

add_executable(any_of_benchmark src/benchmarks/any_of_benchmark.cpp)
target_link_libraries(any_of_benchmark PUBLIC simdalgorithm benchmark::benchmark)
target_compile_features(any_of_benchmark PUBLIC cxx_std_20)


cmake_print_variables(CMAKE_CXX_COMPILER_ID CMAKE_CXX_COMPILER_VERSION)

configure_file(${CMAKE_SOURCE_DIR}/CTestCustom.cmake ${CMAKE_BINARY_DIR})
