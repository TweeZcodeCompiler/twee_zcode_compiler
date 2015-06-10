cmake_minimum_required(VERSION 3.2)
project(TweeParser)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wno-deprecated-register")

find_package(BISON 3.0.2 REQUIRED)
find_package(FLEX 2.5.35 REQUIRED)

BISON_TARGET(TweeParser twee.yy ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedTweeParser.cpp)
FLEX_TARGET(TweeScanner twee.ll  ${CMAKE_CURRENT_SOURCE_DIR}/GeneratedTweeLexer.cpp)
ADD_FLEX_BISON_DEPENDENCY(TweeScanner TweeParser)

file(GLOB CPP_FILES *.cpp)

add_library(${PROJECT_NAME} STATIC ${CPP_FILES} ${BISON_TweeParser_OUTPUTS} ${FLEX_TweeScanner_OUTPUTS} TweeParser.cpp)
