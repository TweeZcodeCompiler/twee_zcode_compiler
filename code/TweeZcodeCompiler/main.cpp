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

    string filePath = fileReader->getFilePathFromArgs(argc,argv);

    string fileContent = fileReader->readFile(filePath);
    cout << fileContent;

    //SIMPLE COMPILER PIPLINE FOR HELLO WORLD
    simpleCompilerPipeline(fileContent);


    //TODO: next step call Lexer with FileContent
    return 0;
}
//Just for Presentation

void simpleCompilerPipeline(string fileContent)
{
    cout << "simple hello world pipeline started";
    //get the hello world string form twee source code
    string token = simpleLexer(fileContent);
    token = "abcdefghijklmnopqurstuvwxyz";

    std::vector<std::bitset<8>> zCode;

    //generate header
    ZCodeHeader *header = new ZCodeHeader();
    header -> setFileLength(800, 99);
    header -> setRoutinesOffset(100);
    header -> setStaticStringsOffset(200);

    std::vector<std::bitset<8>> *zByteHeader = header->getHeaderBits();
    zCode.insert(zCode.end(),zByteHeader->begin(),zByteHeader->end());


    //generate zcode for hello world string
    RoutineGenerator routineGenerator;
    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.printPrintRoutine(token);
    zCode.insert(zCode.end(),zByteCodePrint.begin(),zByteCodePrint.end());


    printHex(zByteCodePrint);

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write("first.z3",zCode);


}


string simpleLexer(string fileContent)
{
    std::string token = fileContent.substr(fileContent.find('\n'),fileContent.length());
    return token;
}

void printHex(std::vector<std::bitset<8>> bitsetList)
{
    cout << endl;
    for(int i = 0;i<bitsetList.size();i++)
    {
        bitset<8> set(bitsetList.at(i));
        cout << hex << set.to_ulong();

    }

}