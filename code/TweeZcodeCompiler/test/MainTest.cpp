//
// Created by Manuel Polzhofer on 07.05.15.
//

#include "MainTest.h"
#include "BinaryFileWriterTest.h"


void MainTest::runAllTest()
{
    //test file writer
    BinaryFileWriterTest binaryFileWriterTest;
    binaryFileWriterTest.testBinaryFileWriter();
}
