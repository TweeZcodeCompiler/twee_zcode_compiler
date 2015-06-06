//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "SimpleCompilerPipeline.h"

#include <TweeParser.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <memory>

using namespace std;

// TODO: check if PASSAGE_GLOB can be replaced with return values
static const string PASSAGE_GLOB = "PASSAGE_PTR",
        JUMP_TABLE_LABEL = "JUMP_TABLE";

string labelForPassage(Passage& passage) {
    stringstream ss;
    ss << "L_" << passage.getPassageName();
    return ss.str();
}

string routineNameForPassage(Passage& passage) {
    stringstream ss;
    ss << "R_" << passage.getPassageName();
    return ss.str();
}

void compilePassage(Passage passage, std::ostream& out) {
    out << ".ROUTINE " << passage.getPassageName() << endl;
    // TODO: go through all links and present options here. code should set PASSAGE_GLOB and return

    out << "\t" << "RET 0" << endl << endl;
}

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

    ofstream out = ofstream("test.zas", ofstream::out);
    // create some dummy passages
    Body body("Testtestest");
    Passage start("start", body);
    start.id = 0;
    Passage passage1("passage1", body);
    passage1.id = 1;
    Passage passage2("passage2", body);
    passage2.id = 2;

    vector<Passage> passages;
    passages.push_back(start);
    passages.push_back(passage1);
    passages.push_back(passage2);


    out << ".GLOBAL " << PASSAGE_GLOB << endl << endl;
    out << ".ROUTINE main" << endl;
    out << "\t" << "CALL start" << endl;

    out << JUMP_TABLE_LABEL << ":" << endl;

    for(vector<Passage>::iterator it = passages.begin(); it != passages.end(); ++it) {
        //out << it->getPassageName() << endl;
        out << "\t" << "JE #" << it->id << " " << PASSAGE_GLOB << " ?(" << labelForPassage(*it)<<  ")" << endl;
    }

    // TODO: error handling in case no matching passage could be found

    for(vector<Passage>::iterator it = passages.begin(); it != passages.end(); ++it) {
        out << labelForPassage(*it) << ":" << endl
            << "\t" << "CALL " << it->getPassageName() << endl
            << "\t" << "JUMP " << JUMP_TABLE_LABEL << endl;
    }

    out << "\t" << "QUIT";

            out << endl << endl
        << "; passages" << endl;

    for(vector<Passage>::iterator it = passages.begin(); it != passages.end(); ++it) {
        compilePassage(*it, out);
    }


    out.close();

    std::vector<std::bitset<8>> zCode;

    //create header and append
    std::vector<std::bitset<8>> zByteHeader = generateHeader();
    zCode.insert(zCode.end(), zByteHeader.begin(), zByteHeader.end());
    log("ZMachine Header generated and added to ZCode");

    //empty space because memory for opcodes starts later
    fillWithBytes(zCode, 0, 295);


    //generate zcode for token string
    RoutineGenerator routineGenerator = RoutineGenerator();
    std::vector<std::bitset<8>> zByteCodePrint = routineGenerator.printPrintRoutine(
            passage.get()->getBody().getContent());
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
