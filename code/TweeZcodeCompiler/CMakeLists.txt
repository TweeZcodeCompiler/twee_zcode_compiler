cmake_minimum_required(VERSION 3.2)
project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)

add_executable(TweeZcodeCompiler ${SOURCE_FILES} FileReader.cpp ZCodeHeader.cpp ZCodeConverter.h FileReader.h ZCodeConverter.cpp ZCodeConverter.h BinaryFileWriter.cpp BinaryFileWriter.h test/BinaryFileWriterTest.cpp test/BinaryFileWriterTest.h test/MainTest.cpp test/MainTest.h RoutineGenerator.cpp RoutineGenerator.h)

