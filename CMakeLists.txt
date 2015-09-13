cmake_minimum_required(VERSION 2.6)
project(variant)

#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++1y -Wall")

enable_testing()

set(SOURCE_FILES main.cpp)
add_executable(variant ${SOURCE_FILES})