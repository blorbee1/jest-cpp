cmake_minimum_required(VERSION 4.2)
project(jest_cpp VERSION 1.0.0 DESCRIPTION "C++ port of jestjs" LANGUAGES CXX)

add_library(jest-cpp INTERFACE)
add_library(JestCpp::JestCpp ALIAS jest-cpp)

target_compile_features(jest-cpp INTERFACE cxx_std_23)
target_include_directories(jest-cpp INTERFACE $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include> $<INSTALL_INTERFACE:include>)

option(JEST_CPP_BUILD_TESTS "Build tests" ON)
option(JEST_CPP_BUILD_EXAMPLES "Build examples" ON)

if (JEST_CPP_BUILD_TESTS)
    enable_testing()
    add_subdirectory(tests)
endif ()

if (JEST_CPP_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif ()