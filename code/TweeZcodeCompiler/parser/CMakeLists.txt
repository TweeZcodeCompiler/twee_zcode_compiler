cmake_minimum_required(VERSION 3.2)
project(TweeParser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

file(GLOB CPP_FILES *.cpp)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES})
