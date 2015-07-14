project(TweeZcodeCompiler)

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -Wextra")
include_directories(${TweeParser_SOURCE_DIR}/include)

set(SOURCE_FILES ZCodeConverter.cpp ZCodeConverter.h
                                             ZCodeHeader.cpp ZCodeHeader.h BinaryFileWriter.cpp BinaryFileWriter.h
                                             RoutineGenerator.cpp RoutineGenerator.h TweeZCodeCompilerPipeline.cpp TweeZCodeCompilerPipeline.h
                                             ZCodeObjects/ZCodeObject.cpp ZCodeObjects/ZCodeObject.h ZCodeObjects/ZCodeContainer.cpp ZCodeObjects/ZCodeContainer.h ZCodeObjects/ZCodeInstruction.cpp ZCodeObjects/ZCodeInstruction.h ZCodeObjects/ZCodeRoutine.cpp ZCodeObjects/ZCodeRoutine.h ZCodeObjects/ZCodeJump.cpp ZCodeObjects/ZCodeJump.h ZCodeObjects/ZCodeMemorySpace.cpp ZCodeObjects/ZCodeMemorySpace.h ZCodeObjects/ZCodePkgAdrrPadding.cpp ZCodeObjects/ZCodePkgAdrrPadding.h
                                             ZCodeObjects/ZCodeCallAdress.cpp ZCodeObjects/ZCodeCallAdress.h ZCodeObjects/ZCodeLabel.cpp ZCodeObjects/ZCodeLabel.h)
add_executable(TweeZcodeCompiler ${SOURCE_FILES} main.cpp ZAssemblyGenerator.cpp ZAssemblyGenerator.h TweeCompiler.cpp TweeCompiler.h
Utils.cpp Utils.h Jumps.cpp Jumps.h OpcodeParameterGenerator.cpp OpcodeParameterGenerator.h AssemblyParser.cpp AssemblyParser.h ITweeCompiler.cpp ITweeCompiler.h ArgsCommand.cpp ArgsCommand.h AsciiConverter.cpp AsciiConverter.h AsciiConverter.cpp AsciiConverter.h)

target_link_libraries (TweeZcodeCompiler TweeParser)


if(AA_FOUND)
	target_link_libraries(TweeZcodeCompiler ${AA_LIBRARY})
else()
	message(STATUS "AA not found, will not build AA interface")
endif()


FIND_PACKAGE(JPEG REQUIRED)
IF(JPEG_FOUND)
  INCLUDE_DIRECTORIES(${JPEG_INCLUDE_DIR})
  TARGET_LINK_LIBRARIES(TweeZcodeCompiler ${JPEG_LIBRARIES})
ENDIF()

add_executable(TestCompiler ${SOURCE_FILES} test/MainTest.cpp test/BinaryFileWriterTest.h test/BinaryFileWriterTest.cpp
        test/RoutineGeneratorTest.h test/RoutineGeneratorTest.cpp test/Test.cpp test/Test.h test/ZCodeHeaderTest.cpp
        test/ZCodeHeaderTest.h AssemblyParser.cpp AssemblyParser.h Jumps.h Jumps.cpp Utils.h Utils.cpp
        OpcodeParameterGenerator.h OpcodeParameterGenerator.cpp )

target_link_libraries(TestCompiler TweeParser)

include_directories("logger")
