//
// Created by Manuel Polzhofer on 07.05.15.
//

#include "MainTest.h"
#include "BinaryFileWriterTest.h"
#include "ZCodeHeaderTest.h"
#include "RoutineGeneratorTest.h"


void MainTest::runAllTest()
{
    //test file writer
    BinaryFileWriterTest binaryFileWriterTest;
    binaryFileWriterTest.testBinaryFileWriter();

    // test zcode header
    ZCodeHeaderTest headerTest;
    headerTest.testHeader();

    //test RoutineGenerator
    RoutineGeneratorTest routineGeneratorTest;
    routineGeneratorTest.testRoutineGenerator();
}