//
// Created by Manuel Polzhofer on 07.05.15.
//

#include "BinaryFileWriterTest.h"




using namespace std;

// TESTING FUNCTION
void BinaryFileWriterTest::testBinaryFileWriter()
{
    vector<bitset<8>> bitVector;
    bitVector.push_back(42);
    bitVector.push_back(13);
    BinaryFileWriter binaryFileWriter;

    binaryFileWriter.write("test1.z8",bitVector);
}