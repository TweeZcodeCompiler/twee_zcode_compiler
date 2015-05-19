//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "SimpleCompilerPipeline.h"

//Just a Simple Compiler Pipeline

void SimpleCompilerPipeline::compile(string fileContent,string zCodeFileName) {


    log("Simple Compiler Pipeline started");
    //get the hello world string from twee source code
    string token = simpleLexer(fileContent);
    log("Generated Token from Twee File");

    std::vector<std::bitset<8>> zCode;


    //create header and append
    std::vector<std::bitset<8>> *zByteHeader = generateHeader();
    zCode.insert(zCode.end(), zByteHeader->begin(), zByteHeader->end());
    log("ZMachine Header generated and added to ZCode");

    //empty space because memory for opcodes starts later
    zCode = fillWithBytes(0,295,zCode);


    //generate zcode for token string
    RoutineGenerator routineGenerator;
    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.printPrintRoutine(token);
    zCode.insert(zCode.end(), zByteCodePrint.begin(), zByteCodePrint.end());
    log("Print Command added to ZCode");

    //printHex(zByteCodePrint);

    //generate empty space
    zCode = fillWithBytes(0,147,zCode);

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write(zCodeFileName, zCode);
    log("ZCode File '"+zCodeFileName+"' generated");

}




string SimpleCompilerPipeline::simpleLexer(string fileContent) {
    std::string token = fileContent.substr(fileContent.find('\n'), fileContent.length());
    return token;
}

void SimpleCompilerPipeline::printHex(std::vector<std::bitset<8>> bitsetList) {
    cout << endl << endl;
    for (int i = 0; i < bitsetList.size(); i++) {
        bitset<8> set(bitsetList.at(i));
        cout << hex << set.to_ulong();
    }
    cout << endl;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::fillWithBytes(int value,int amountOfBytes,std::vector<std::bitset<8>> bitVector)
{
    for (size_t i = 0; i < amountOfBytes; i++) {
        bitVector.insert(bitVector.end(), value);
    }
    return bitVector;

}

void SimpleCompilerPipeline::log(string message)
{
    cout << "Compiler: " << message << " . . ." <<  "\n";
}


std::vector<std::bitset<8>> *SimpleCompilerPipeline::generateHeader()
{
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
    header->setFileLength(516, 6861);

    std::vector<std::bitset<8>> *zByteHeader = header->getHeaderBits();
    return zByteHeader;
}
