//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "SimpleCompilerPipeline.h"

#include <TweeParser.h>
#include <memory>

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
    std::vector<std::bitset<8>> *zByteHeader = generateHeader();
    zCode.insert(zCode.end(), zByteHeader->begin(), zByteHeader->end());
    log("ZMachine Header generated and added to ZCode");

    //empty space because memory for opcodes starts later
    zCode = fillWithBytes(0, 295, zCode);


    //generate zcode for token string
    RoutineGenerator routineGenerator;
    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.printPrintRoutine(
            passage.get()->getBody().getContent());
    zCode.insert(zCode.end(), zByteCodePrint.begin(), zByteCodePrint.end());
    log("Print Command added to ZCode");

    //calculate fileSize
    int fileSize = calculateFileSize(zCode);

    zCode = addFileSizeToHeader(zCode, fileSize);

    //generate empty space for padding
    int empty = fileSize - zCode.size();
    zCode = fillWithBytes(0, (empty > 0) ? empty : 0, zCode);

    BinaryFileWriter binaryFileWriter;
    binaryFileWriter.write(zCodeFileName, zCode);
    log("ZCode File '" + zCodeFileName + "' generated");

}

int SimpleCompilerPipeline::calculateFileSize(std::vector<std::bitset<8>> zCode) {
    int filesize = ((zCode.size() + 8) / 8) * 8;
    return filesize;

}

std::vector<std::bitset<8>> SimpleCompilerPipeline::addFileSizeToHeader(std::vector<std::bitset<8>> zCode,
                                                                        int fileSize) {
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
    for (int i = 0; i < bitsetList.size(); i++) {
        bitset<8> set(bitsetList.at(i));
        cout << hex << set.to_ulong();
    }
    cout << endl;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::fillWithBytes(int value, int amountOfBytes,
                                                                  std::vector<std::bitset<8>> bitVector) {
    for (size_t i = 0; i < amountOfBytes; i++) {
        bitVector.insert(bitVector.end(), value);
    }
    return bitVector;

}

void SimpleCompilerPipeline::log(string message) {
    cout << "Compiler: " << message << " . . ." << "\n";
}


std::vector<std::bitset<8>> *SimpleCompilerPipeline::generateHeader() {
    //generate header
    header = new ZCodeHeader();
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
    header->setFileLength(2064, 6861);

    std::vector<std::bitset<8>> *zByteHeader = header->getHeaderBits();
    return zByteHeader;
}
