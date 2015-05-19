#include <iostream>
#include <vector>
#include "FileReader.h"
#include "test/MainTest.h"
#include "RoutineGenerator.h"
#include "BinaryFileWriter.h"
#include "ZCodeHeader.h"

using namespace std;

string simpleLexer(string fileContent);

void simpleCompilerPipeline(string fileContent);

void printHex(std::vector<std::bitset<8>> bitsetList);

int main(int argc, char *argv[]) {
    MainTest test;
    // test.runAllTest();

    FileReader *fileReader = new FileReader();

    string filePath = fileReader->getFilePathFromArgs(argc, argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;

    //SIMPLE COMPILER PIPLINE FOR HELLO WORLD
    simpleCompilerPipeline(fileContent);


    //TODO: next step call Lexer with FileContent
    return 0;
}
//Just for Presentation

void simpleCompilerPipeline(string fileContent) {
    cout << "\n\nsimple hello world pipeline started";

    //get the hello world string from twee source code
    string token = simpleLexer(fileContent);
    token = "Dies ist ein Text";

    std::vector<std::bitset<8>> zCode;

    //generate header
    ZCodeHeader *header = new ZCodeHeader();
    header->baseOfHighMem = 358;
    header->initValOfPC = 1;
    header->packedAddressOfMain = 103;
    header->locOfDict = 328;
    header->locOfObjTable = 266;
    header->locOfGlobVarTable = 258;
    header->baseOfStatMem = 328;

    header->locOfAbbrTable = 66;

    header->setRoutinesOffset(128);         // random value
    header->setStaticStringsOffset(128);    // random value

    //generate zcode for hello world string
    RoutineGenerator routineGenerator;
    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.printPrintRoutine(token);


    printHex(zByteCodePrint);

    header->setFileLength(516, 6861);

    std::vector<std::bitset<8>> *zByteHeader = header->getHeaderBits();

    zCode.insert(zCode.end(), zByteHeader->begin(), zByteHeader->end());
    for (size_t i = 0; i < 295; i++) {
        zCode.insert(zCode.end(), 0);
    }
    zCode.insert(zCode.end(), zByteCodePrint.begin(), zByteCodePrint.end());
    for (size_t i = 0; i < 147; i++) {
        zCode.insert(zCode.end(), 0);
    }

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write("hello_world.z8", zCode);
}


string simpleLexer(string fileContent) {
    std::string token = fileContent.substr(fileContent.find('\n'), fileContent.length());
    return token;
}

void printHex(std::vector<std::bitset<8>> bitsetList) {
    cout << endl << endl;
    for (int i = 0; i < bitsetList.size(); i++) {
        bitset<8> set(bitsetList.at(i));
        cout << hex << set.to_ulong();
    }
    cout << endl;
}