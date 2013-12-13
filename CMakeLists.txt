cmake_minimum_required(VERSION 2.8)
project(display)

set(CMAKE_C_FLAGS "-O0 -ffast-math -g -Werror")
set(CMAKE_CXX_FLAGS "-std=c++0x ${CMAKE_C_FLAGS}")

set(CMAKE_EXE_LINKER_FLAGS "-rdynamic")

add_definitions(-x c++ -std=c++0x -g -rdynamic -Wall)
link_libraries(stdc++ rt)
add_library(mandrews_static STATIC testing.cpp socket.cpp)
add_library(mandrews)
target_link_libraries(mandrews mandrews_static)
