project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra")
include_directories(${TweeParser_SOURCE_DIR}/include)

set(SOURCE_FILES ZCodeConverter.cpp ZCodeConverter.h
                                             ZCodeHeader.cpp ZCodeHeader.h BinaryFileWriter.cpp BinaryFileWriter.h
                                             RoutineGenerator.cpp RoutineGenerator.h TweeZCodeCompilerPipeline.cpp TweeZCodeCompilerPipeline.h)
add_executable(TweeZcodeCompiler ${SOURCE_FILES} main.cpp ZAssemblyGenerator.cpp ZAssemblyGenerator.h TweeCompiler.cpp TweeCompiler.h
Utils.cpp Utils.h Jumps.cpp Jumps.h OpcodeParameterGenerator.cpp OpcodeParameterGenerator.h AssemblyParser.cpp AssemblyParser.h ParamStructs.h ITweeCompiler.cpp ITweeCompiler.h)

target_link_libraries (TweeZcodeCompiler TweeParser)

add_executable(TestCompiler ${SOURCE_FILES} test/MainTest.cpp test/BinaryFileWriterTest.h test/BinaryFileWriterTest.cpp
test/RoutineGeneratorTest.h test/RoutineGeneratorTest.cpp test/Test.cpp test/Test.h test/ZCodeHeaderTest.cpp test/ZCodeHeaderTest.h AssemblyParser.cpp AssemblyParser.h test/LocalVariablesTest.cpp test/LocalVariablesTest.h)
target_link_libraries(TestCompiler TweeParser)

include_directories("logger")
