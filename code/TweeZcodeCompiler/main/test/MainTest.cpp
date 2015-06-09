//
// Created by Manuel Polzhofer on 07.05.15.
//
#include "BinaryFileWriterTest.h"
#include "ZCodeHeaderTest.h"
#include "RoutineGeneratorTest.h"

int main() {
    //Add tests to Testframework
    Test::addTest("BinaryFileWriter", new BinaryFileWriterTest());
    Test::addTest("RoutineGenerator", new RoutineGeneratorTest());
    Test::addTest("ZCodeHeader", new ZCodeHeaderTest());
    //Execute tests
    Test::executeAllTests(false);
    //call delete for each test
    Test::deleteTests();
    return 0;
}
