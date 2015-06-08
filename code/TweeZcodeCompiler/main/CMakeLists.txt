project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

include_directories(${TweeParser_SOURCE_DIR}/include)

set(SOURCE_FILES main.cpp)

add_executable(TweeZcodeCompiler ${SOURCE_FILES} ZCodeConverter.cpp ZCodeConverter.h
    ZCodeHeader.cpp ZCodeHeader.h BinaryFileWriter.cpp BinaryFileWriter.h test/BinaryFileWriterTest.cpp
    test/BinaryFileWriterTest.h test/MainTest.cpp test/MainTest.h test/ZCodeHeaderTest.cpp test/ZCodeHeaderTest.h RoutineGenerator.cpp RoutineGenerator.h test/RoutineGeneratorTest.cpp test/RoutineGeneratorTest.h SimpleCompilerPipeline.cpp SimpleCompilerPipeline.h Jumps.cpp Jumps.h OpcodeParameterGenerator.cpp OpcodeParameterGenerator.h)

target_link_libraries (TweeZcodeCompiler TweeParser)

