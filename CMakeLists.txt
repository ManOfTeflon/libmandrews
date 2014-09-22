cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)

project(display)

set(CMAKE_C_FLAGS "-O3 -ffast-math -g -Werror")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_C_FLAGS}")

add_library(mandrews STATIC testing.cpp socket.cpp semaphore.cpp output.cpp)
target_link_libraries(mandrews rt pthread readline)

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_executable(test_array tests/array.cpp)
target_link_libraries(test_array mandrews)
