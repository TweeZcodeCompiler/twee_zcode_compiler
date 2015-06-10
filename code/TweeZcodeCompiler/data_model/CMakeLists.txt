cmake_minimum_required(VERSION 3.2)
project(TweeDataModel)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
include_directories(${TweeParser_SOURCE_DIR}/include)

set(SOURCE_FILES

include/TweeFile.h TweeFile.cpp

include/Passage/Passage.h Passage.cpp
include/Passage/Head.h Head.cpp
include/Passage/Body.h Body.cpp

include/Passage/Body/BodyPart.h
include/Passage/Body/Link.h Link.cpp
include/Passage/Body/Text.h Text.cpp
include/Passage/Body/FormattedText.h FormattedText.cpp

main.cpp

)
