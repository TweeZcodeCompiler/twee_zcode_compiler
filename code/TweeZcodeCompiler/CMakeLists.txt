project(root)

cmake_minimum_required(VERSION 3.2)

set(CMAKE_MODULE_PATH "${root_SOURCE_DIR}")
include(FindAA)

add_subdirectory(parser)
add_subdirectory(main)

include_directories(${TweeParser_SOURCE_DIR}/include)

