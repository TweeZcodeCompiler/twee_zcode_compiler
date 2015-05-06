#include <iostream>
#include "FileReader.h"
#include "BinaryFileWriter.h"

using namespace std;

// TESTING FUNCTION
void testBinaryFileWriter()
{
    vector<bitset<16>> bitVector;
    bitVector.push_back(42);
    BinaryFileWriter binaryFileWriter;

    binaryFileWriter.write("test.z8",&bitVector);
}

int main(int argc, char *argv[]) {

    testBinaryFileWriter();
    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;
    //TODO: next step call Lexer with FileContent
    return 0;
}
