//
// Created by Manuel Polzhofer on 19.05.15.
//

#include "SimpleCompilerPipeline.h"

#include <TweeParser.h>
#include <fstream>
#include <memory>
#include "AssemblyParser.h"

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

    //create header
    ZCodeHeader header = ZCodeHeader();

    //create memory sections
    vector<bitset<8>> dynamicMemory = generateDynamicMemory(header, 0x3f);
    vector<bitset<8>> staticMemory = generateStaticMemory(header, (int) (0x3f + dynamicMemory.size()));
    vector<bitset<8>> highMemory = generateHighMemory(header,
                                                      (int) (0x3f + staticMemory.size() + dynamicMemory.size()));

    //init header
    header.setRoutinesOffset(88);
    header.setStaticStringsOffset(99);
    header.setFileLength(3, 52);
    header.baseOfStatMem = (uint16_t) (0x3f + dynamicMemory.size());
    header.baseOfHighMem = (uint16_t) (0x3f + dynamicMemory.size() + staticMemory.size());
    header.initValOfPC = header.baseOfHighMem;
    vector<bitset<8>> headerMemory = header.getHeaderBits();

    //concat memory sections
    vector<bitset<8>> zCode = vector<bitset<8>>();
    Utils::append(zCode, headerMemory);
    Utils::append(zCode, dynamicMemory);
    Utils::append(zCode, staticMemory);
    Utils::append(zCode, highMemory);



    RoutineGenerator::resolveCallInstructions(zCode);



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

std::vector<std::bitset<8>> SimpleCompilerPipeline::generateDynamicMemory(ZCodeHeader &header, size_t offset) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    //abbervation strings
    Utils::fillWithBytes(akk, 0, 2);
    //abbervation table
    header.locOfAbbrTable = (uint16_t) (offset + akk.size());
    Utils::fillWithBytes(akk, 0, 0xc0);
    //property defaults
    Utils::fillWithBytes(akk, 0, 0x3e);
    //objects
    header.locOfObjTable = (uint16_t) (offset + akk.size());
    Utils::fillWithBytes(akk, 0, 0x5a3);
    //globalVariables
    header.locOfGlobVarTable = (uint16_t) (offset + akk.size());
    vector<bitset<8>> vars = printGlobalTable((int) (offset + akk.size()));
    akk.insert(akk.end(), vars.begin(), vars.end());
    return akk;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::generateStaticMemory(ZCodeHeader &header, size_t offset) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    //grammar table
    Utils::fillWithBytes(akk, 0, 0x55f);
    //actions table
    Utils::fillWithBytes(akk, 0, 0xac);
    //preactions table
    Utils::fillWithBytes(akk, 0, 0xae);
    //adjectives table
    Utils::fillWithBytes(akk, 0, 0x4c);
    //dictionary
    Utils::fillWithBytes(akk, 0, 0x7bd);
    return akk;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::generateHighMemory(ZCodeHeader &header, size_t offset) {
    vector<bitset<8>> highMemoryZcode = vector<bitset<8>>();

    // this part creates call to first routine
    RoutineGenerator callToMainroutineGenerator = RoutineGenerator(offset);
    vector<bitset<8>> routine = callToMainroutineGenerator.getRoutine();
    Utils::append(highMemoryZcode, routine);

/*
    RoutineGenerator testRoutineGenerator = RoutineGenerator("main", 0, highMemoryZcode, offset);
    testRoutineGenerator.printString("Dies ist 1 Test");
    testRoutineGenerator.newLine();
    testRoutineGenerator.printString("1: gehe in den Wald!");
    testRoutineGenerator.newLine();
    testRoutineGenerator.printString("2: gehe in die Stadt!");
    testRoutineGenerator.newLine();
    testRoutineGenerator.printString("3: gehe zum Weg!");
    testRoutineGenerator.readChar(0x10);
    testRoutineGenerator.jumpEquals("w", true, 0x10, 49, true, false);
    testRoutineGenerator.jumpEquals("s", true, 0x10, 50, true, false);
    testRoutineGenerator.jumpEquals("weg", true, 0x10, 51, true, false);
    testRoutineGenerator.printString("Keine valide eingabe!");
    testRoutineGenerator.quitRoutine();
    testRoutineGenerator.newLabel("w");
    testRoutineGenerator.printString("korrekt");
    testRoutineGenerator.callRoutine("wald");
    testRoutineGenerator.quitRoutine();
    testRoutineGenerator.newLabel("s");
    testRoutineGenerator.printString("korrekt");
    testRoutineGenerator.callRoutine("stadt");
    testRoutineGenerator.quitRoutine();
    testRoutineGenerator.newLabel("weg");
    testRoutineGenerator.printString("korrekt");
    testRoutineGenerator.callRoutine("weg");
    testRoutineGenerator.quitRoutine();


    vector<bitset<8>> testRoutine = testRoutineGenerator.getRoutine();
    Utils::append(highMemoryZcode, testRoutine);
    */


    AssemblyParser assemblyParser;

    assemblyParser.readAssembly("hello.zap",highMemoryZcode,offset);
    //assemblyParser.readAssembly("haus.zap",highMemoryZcode,offset);

/*
    RoutineGenerator routine1 = RoutineGenerator("wald", 0, highMemoryZcode, offset);
    routine1.newLine();
    routine1.newLine();
    routine1.printString("Dies ist der Wald!");
    routine1.newLine();
    routine1.callRoutine("main");
    routine1.quitRoutine();

    vector<bitset<8>> vroutine1 = routine1.getRoutine();
    Utils::append(highMemoryZcode, vroutine1);

    RoutineGenerator routine2 = RoutineGenerator("stadt", 0, highMemoryZcode, offset);
    routine2.newLine();
    routine2.newLine();
    routine2.printString("Dies ist die Stadt!");
    routine2.newLine();
    routine2.callRoutine("main");
    routine2.quitRoutine();

    vector<bitset<8>> vroutine2 = routine2.getRoutine();
    Utils::append(highMemoryZcode, vroutine2);

    RoutineGenerator routine3 = RoutineGenerator("weg", 0, highMemoryZcode, offset);
    routine3.newLine();
    routine3.newLine();
    routine3.printString("Dies der Weg!");
    routine3.newLine();
    routine3.callRoutine("main");
    routine3.quitRoutine();

    vector<bitset<8>> vroutine3 = routine3.getRoutine();
    Utils::append(highMemoryZcode, vroutine3); */

    return highMemoryZcode;
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
        cout << hex << set.to_ulong() << endl;
    }
    cout << endl;
}

std::vector<std::bitset<8>> SimpleCompilerPipeline::printGlobalTable(int offset) {
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

void SimpleCompilerPipeline::log(string message) {
    cout << "Compiler: " << message << " . . ." << "\n";
}
