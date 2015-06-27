//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "TweeZCodeCompilerPipeline.h"

#include <TweeParser.h>
#include <fstream>
#include <iostream>
#include <memory>
#include <map>
#include <TweeFile.h>
#include "AssemblyParser.h"
#include "ZCodeObjects/ZCodeContainer.h"
#include "ZCodeObjects/ZCodeMemorySpace.h"
#include <sstream>
#include <plog/Log.h>
#include <stdint-gcc.h>

using namespace std;

void TweeZCodeCompilerPipeline::compile(string filename, string zCodeFileName, ITweeCompiler& tweeCompiler,bool isTwee) {

    log("Simple Compiler Pipeline started");

    stringstream buffer;
    if(isTwee) { //source file is twee file
        ifstream inputFile(filename);

        Twee::TweeParser parser(&inputFile);

        std::unique_ptr<TweeFile> tweeFile;
        try {
            tweeFile = parser.parse();
        } catch (const Twee::ParseException &e) {
            log("Parse error");
            throw e;
        }

        log("Parsed twee file");
        tweeCompiler.compile(*tweeFile, buffer);
    } else {
        //source file is assembly file
        std::ifstream in(filename);
        buffer << in.rdbuf();
    }



    ZCodeContainer zcode = ZCodeContainer();

    //create header
    ZCodeHeader *header = new ZCodeHeader();
    zcode.add(header);

    //create dynamicMemory
    ZCodeContainer *dynamicMemory = new ZCodeContainer();
    ZCodeMemorySpace *globalVariablesTable = new ZCodeMemorySpace((0xff - 0x10)*2);// Global Var Table
    dynamicMemory->add(globalVariablesTable);
    zcode.add(dynamicMemory);

    //create staticMemory
    ZCodeContainer *staticMemory = new ZCodeContainer();
    zcode.add(staticMemory);

    //create hight Memory
    ZCodeContainer *highMemory =new ZCodeContainer();
    zcode.add(highMemory);

    //parse
    AssemblyParser parser = AssemblyParser();
    parser.readAssembly(buffer,dynamicMemory,staticMemory,highMemory);

    //init header
    header->setRoutinesOffset(88);
    header->setStaticStringsOffset(99);
    header->setFileLength(3, 52);
    header->locOfGlobVarTable = globalVariablesTable->getOffset();
    header->baseOfStatMem = (uint16_t) (staticMemory->getOffset());
    header->baseOfHighMem = (uint16_t) (highMemory->getOffset());
    header->initValOfPC = header->baseOfHighMem;

    //concat memory sections

    vector<bitset<8>> zCode;
    zcode.print(zCode);


    //calculate fileSize
    size_t fileSize = Utils::calculateNextPackageAddress(zCode.size());
    zCode = addFileSizeToHeader(zCode, fileSize);

    //generate empty space for padding
    size_t empty = fileSize - zCode.size();
    Utils::fillWithBytes(zCode, 0, (empty > 0) ? empty : 0);

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write(zCodeFileName, zCode);
    log("ZCode File '" + zCodeFileName + "' generated");
}

std::vector<std::bitset<8>> TweeZCodeCompilerPipeline::addFileSizeToHeader(std::vector<std::bitset<8>> zCode,
                                                                        size_t fileSize) {
    //change fileSize in header
    bitset<16> shortVal(fileSize / 8);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }
    zCode[ZCodeHeader::HEADER_FILE_SIZE_POSITION] = firstHalf;
    zCode[ZCodeHeader::HEADER_FILE_SIZE_POSITION + 1] = secondHalf;
    return zCode;
}


void TweeZCodeCompilerPipeline::printHex(std::vector<std::bitset<8>> bitsetList) {

    for (unsigned int i = 0; i < bitsetList.size(); i++) {
        bitset<8> set(bitsetList.at(i));
        LOG_DEBUG << hex << set.to_ulong();
    }
    LOG_DEBUG;
}

std::vector<std::bitset<8>> TweeZCodeCompilerPipeline::printGlobalTable(int offset) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    for (int i = 0; i < (0xff - 0x10); i++) {
        int adr = offset + (0xff - 0x10) * 2 + i * 100;
        bitset<8> one = bitset<8>((unsigned long long int) adr / 255);
        bitset<8> two = bitset<8>((unsigned long long int) adr % 256);
        akk.push_back(one);
        akk.push_back(two);
    }
    for (int i = 0; i < (0xff - 0x10); i++) {
        akk.push_back(bitset<8>(4));
        akk.push_back(bitset<8>(0));
        Utils::fillWithBytes(akk, 0, 96);
        akk.push_back(bitset<8>(0x80));
        akk.push_back(bitset<8>(0));
    }

    return akk;
}

void TweeZCodeCompilerPipeline::log(string message) {
    LOG_DEBUG << "Compiler: " << message << " . . ." << "\n";
}
