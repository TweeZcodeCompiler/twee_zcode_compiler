cmake_minimum_required(VERSION 3.2)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_subdirectory(parser)
add_subdirectory(data_model)
add_subdirectory(main)

include_directories(${TweeParser_SOURCE_DIR}/include)
