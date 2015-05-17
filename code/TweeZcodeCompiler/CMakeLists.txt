cmake_minimum_required(VERSION 3.2)
project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

set(SOURCE_FILES main.cpp)
add_executable(TweeZcodeCompiler ${SOURCE_FILES} FileReader.cpp FileReader.h ZCodeConverter.cpp ZCodeConverter.h
    ZCodeHeader.cpp ZCodeHeader.h BinaryFileWriter.cpp BinaryFileWriter.h test/BinaryFileWriterTest.cpp
    test/BinaryFileWriterTest.h test/MainTest.cpp test/MainTest.h test/ZCodeHeaderTest.cpp test/ZCodeHeaderTest.h RoutineGenerator.cpp RoutineGenerator.h test/RoutineGeneratorTest.cpp test/RoutineGeneratorTest.h)
add_executable(BitsetOperations  bithandling/examples_bitset_operations.cpp)
add_executable(BitOperations bithandling/examples_bit_operations.cpp)
add_executable(BitAccess bithandling/examples_bit_access.cpp)

