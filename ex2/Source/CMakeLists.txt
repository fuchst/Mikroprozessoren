cmake_minimum_required(VERSION 3.3)
project(MicProc2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES cacheline.cpp)
add_executable(MicProc2 ${SOURCE_FILES})