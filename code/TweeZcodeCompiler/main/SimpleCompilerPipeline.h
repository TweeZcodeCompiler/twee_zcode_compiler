//
// Created by Manuel Polzhofer on 19.05.15.
//

#ifndef TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H
#define TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H

#include <iostream>
#include <vector>
#include "FileReader.h"
#include "RoutineGenerator.h"
#include "BinaryFileWriter.h"
#include "ZCodeHeader.h"

class SimpleCompilerPipeline {

public:
    void compile(string fileContent,string zCodeFileName);

private:
    string simpleLexer(string fileContent);
    void printHex(std::vector<std::bitset<8>> bitsetList);
    std::vector<std::bitset<8>> generateHeader();
    std::vector<std::bitset<8>> fillWithBytes(int value,int amountOfBytes,std::vector<std::bitset<8>> bitVector);
    std::vector<std::bitset<8>> addFileSizeToHeader(std::vector<std::bitset<8>> zCode, int fileSize);
    int calculateFileSize(std::vector<std::bitset<8>> zCode);
    void log(string message);



};


#endif //TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H
