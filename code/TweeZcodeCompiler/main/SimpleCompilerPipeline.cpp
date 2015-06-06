//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "SimpleCompilerPipeline.h"

#include <TweeParser.h>
#include <fstream>
#include <memory>
#include <bits/stl_map.h>

using namespace std;

//Just a Simple Compiler Pipeline
void SimpleCompilerPipeline::compile(string filename, string zCodeFileName) {

    log("Simple Compiler Pipeline started");

    ifstream inputFile(filename);

    Twee::TweeParser parser(&inputFile);

    std::unique_ptr<Passage> passage;
    try {
        passage.reset(parser.parse());
    } catch (Twee::ParseException e) {
        log("Parse error");
        throw e;
    }

    log("Parsed twee file");

    std::vector<std::bitset<8>> zCode;

    //create header and append
    std::vector<std::bitset<8>> zByteHeader = generateHeader();
    zCode.insert(zCode.end(), zByteHeader.begin(), zByteHeader.end());
    log("ZMachine Header generated and added to ZCode");

    //empty space because memory for opcodes starts later
    fillWithBytes(zCode, 0, 295);


    //generate zcode for token string
    RoutineGenerator routineGenerator = RoutineGenerator(167);

    routineGenerator.print("Test - ");
    routineGenerator.addLabel("one");
    //routineGenerator.jump("two");
    routineGenerator.print(passage.get()->getBody().getContent());
    routineGenerator.newLine();
    routineGenerator.jumpZero(3, false, "two", true);

    routineGenerator.print(passage.get()->getBody().getContent());
    routineGenerator.print("zzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzzz");
    routineGenerator.print(passage.get()->getBody().getContent());

    routineGenerator.jumpZero(2, false, "three", true);
    routineGenerator.jumpZero(0, false, "one", true);
    routineGenerator.jumpZero(0, false, "three", true);
    routineGenerator.jumpZero(0, false, "three", true);

    routineGenerator.addLabel("two");
    routineGenerator.print("Ende");

    routineGenerator.addLabel("three");

    routineGenerator.print("Bla");

    routineGenerator.quitRoutine();

    /*routineGenerator.jump("print");
    routineGenerator.addBranch("quit");
    routineGenerator.quitRoutine();
    routineGenerator.addLabel("print");
    routineGenerator.print(passage.get()->getBody().getContent());
    routineGenerator.newLine();
    routineGenerator.print(passage.get()->getBody().getContent());
    routineGenerator.jumpZero("quit");*/



    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.getRoutine();
    zCode.insert(zCode.end(), zByteCodePrint.begin(), zByteCodePrint.end());
    log("Print Command added to ZCode");

    //calculate fileSize
    size_t fileSize = calculateFileSize(zCode);

    zCode = addFileSizeToHeader(zCode, fileSize);

    //generate empty space for padding
    size_t empty = fileSize - zCode.size();
    fillWithBytes(zCode, 0, (empty > 0) ? empty : 0);

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write(zCodeFileName, zCode);
    log("ZCode File '" + zCodeFileName + "' generated");
}

size_t SimpleCompilerPipeline::calculateFileSize(std::vector<std::bitset<8>> zCode) {
    return ((zCode.size() + 8) / 8) * 8;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::addFileSizeToHeader(std::vector<std::bitset<8>> zCode,
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


void SimpleCompilerPipeline::printHex(std::vector<std::bitset<8>> bitsetList) {
    cout << endl << endl;
    for (unsigned int i = 0; i < bitsetList.size(); i++) {
        bitset<8> set(bitsetList.at(i));
        cout << hex << set.to_ulong();
    }
    cout << endl;
}

void SimpleCompilerPipeline::fillWithBytes(std::vector<std::bitset<8>>& bitVector, uint8_t value, unsigned long amountOfBytes) {
    for (size_t i = 0; i < amountOfBytes; i++) {
        bitVector.insert(bitVector.end(), value);
    }
}

void SimpleCompilerPipeline::log(string message) {
    cout << "Compiler: " << message << " . . ." << "\n";
}


std::vector<std::bitset<8>> SimpleCompilerPipeline::generateHeader() {
    //generate header
    ZCodeHeader header;
    header.baseOfHighMem = 358;
    header.initValOfPC = 1;
    header.packedAddressOfMain = 103;
    header.locOfDict = 328;
    header.locOfObjTable = 266;
    header.locOfGlobVarTable = 258;
    header.baseOfStatMem = 328;

    header.locOfAbbrTable = 66;

    header.setRoutinesOffset(128);         // random value
    header.setStaticStringsOffset(128);    // random value
    header.setFileLength(2064, 6861);
    
    return header.getHeaderBits();
}
