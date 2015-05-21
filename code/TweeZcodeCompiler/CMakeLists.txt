cmake_minimum_required(VERSION 3.2)
project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(parser)
add_subdirectory(main)

include_directories(${TweeParser_SOURCE_DIR}/include)

add_executable(BitsetOperations  bithandling/examples_bitset_operations.cpp)
add_executable(BitOperations bithandling/examples_bit_operations.cpp)
add_executable(BitAccess bithandling/examples_bit_access.cpp)
