cmake_minimum_required(VERSION 3.2)
project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(TweeZcodeCompiler ${SOURCE_FILES} FileReader.cpp FileReader.h ZCodeHeader.h ZCodeHeader.cpp)