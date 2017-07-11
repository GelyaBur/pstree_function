cmake_minimum_required(VERSION 2.8)

#add_executable(open1 open1.c)
project(lib)

add_library(lib STATIC error_functions.c)

#add_definitions(-g)

#link_directories(/home/gelya/usr/local/lib)

#include_directories(./lib)

#target_link_libraries(log PocoFoundation)
