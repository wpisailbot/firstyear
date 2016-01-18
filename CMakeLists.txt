cmake_minimum_required(VERSION 2.8)

project(Sailbot)

enable_testing()

# Compiler Options
set(CMAKE_BUILD_TYPE Debug)
set(CMAKE_CXX_COMPILER "clang++-3.6")
set(CMAKE_C_COMPILER "clang-3.6")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Wextra -Werror -std=c++1y")

link_directories("/usr/src/gtest/build")

link_libraries(rt pthread)
include_directories("include/")

# gtest tests.
add_executable(queue_test "src/queue-test.cc")
target_link_libraries(queue_test gtest gtest_main)

add_test(AllTests queue_test)

add_subdirectory(test)
