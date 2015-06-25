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
#include "ITweeCompiler.h"

class TweeZCodeCompilerPipeline {

public:
    void compile(std::string filename, std::string zCodeFileName, ITweeCompiler &tweeCompiler,bool isTwee);

private:
    std::vector<std::bitset<8>> generateDynamicMemory(ZCodeHeader &header, size_t offset);

    std::vector<std::bitset<8>> generateStaticMemory(ZCodeHeader &header, size_t offset);

    std::vector<std::bitset<8>> generateHighMemory(ZCodeHeader &header, size_t offset, std::istream& instructionsInput);

    std::vector<std::bitset<8>> printGlobalTable(int offset);

    void printHex(std::vector<std::bitset<8>> bitsetList);

    std::vector<std::bitset<8>> addFileSizeToHeader(std::vector<std::bitset<8>> zCode, size_t fileSize);

    void log(std::string message);
};


#endif //TWEEZCODECOMPILER_SIMPLECOMPILERPIPELINE_H
