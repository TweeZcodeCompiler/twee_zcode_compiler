//
// Created by Manuel Polzhofer on 07.05.15.
//

#include "BinaryFileWriterTest.h"


using namespace std;

// TESTING FUNCTION
void BinaryFileWriterTest::runTest() {
    ignoreTestResult();
    vector<bitset<8>> bitVector;
    bitVector.push_back(42);
    bitVector.push_back(13);
    BinaryFileWriter binaryFileWriter;

    binaryFileWriter.write("test1.z8", bitVector);
    customTestResult("A file (\"test1.z8\") with two byte (42 and 13) should be generated.");
}