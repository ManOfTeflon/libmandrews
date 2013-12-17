cmake_minimum_required(VERSION 2.8)

SET(CMAKE_C_COMPILER clang)
SET(CMAKE_CXX_COMPILER clang++)

project(display)

set(CMAKE_C_FLAGS "-O3 -ffast-math -g -Werror")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_C_FLAGS}")

add_library(mandrews_static STATIC testing.cpp socket.cpp semaphore.cpp)
target_link_libraries(mandrews_static rt pthread readline)
add_library(mandrews)
target_link_libraries(mandrews mandrews_static)
