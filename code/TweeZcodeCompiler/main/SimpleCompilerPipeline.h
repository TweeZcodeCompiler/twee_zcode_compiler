//
// Created by Manuel Polzhofer on 19.05.15.
//

#ifndef TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H
#define TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H

#include <iostream>
#include <vector>
#include "RoutineGenerator.h"
#include "BinaryFileWriter.h"
#include "ZCodeHeader.h"

class SimpleCompilerPipeline {

public:
    void compile(std::string fileContent, std::string zCodeFileName);

private:
    void printHex(std::vector<std::bitset<8>> bitsetList);
    std::vector<std::bitset<8>> generateHeader();
    void fillWithBytes(std::vector<std::bitset<8>>& bitVector, uint8_t value, unsigned long amountOfBytes);
    std::vector<std::bitset<8>> addFileSizeToHeader(std::vector<std::bitset<8>> zCode, size_t fileSize);
    size_t calculateFileSize(std::vector<std::bitset<8>> zCode);
    void log(std::string message);
};


#endif //TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H
