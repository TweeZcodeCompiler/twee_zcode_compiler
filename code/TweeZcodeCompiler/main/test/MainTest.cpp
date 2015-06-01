//
// Created by Manuel Polzhofer on 07.05.15.
//
#include "BinaryFileWriterTest.h"
#include "ZCodeHeaderTest.h"
#include "RoutineGeneratorTest.h"

int main()
{

    Test::addTest("BinaryFileWriter", new BinaryFileWriterTest());
    Test::addTest("RoutineGenerator", new RoutineGeneratorTest());
    Test::addTest("ZCodeHeader",new ZCodeHeaderTest());
    Test::executeAllTests(false);
    Test::deleteTests();
    return 0;
}
