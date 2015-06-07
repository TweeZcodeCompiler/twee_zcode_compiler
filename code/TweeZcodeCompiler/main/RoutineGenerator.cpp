// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include "ZCodeHeader.h"
#include "Utils.h"
#include <iostream>

using std::vector;
using std::bitset;

std::vector<std::bitset<8>> RoutineGenerator::printPrintStringInstruction(std::string stringToPrint) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);
    unsigned long len = zsciiString.size();
    for (int i = 0; i < len; i++) {
        if (i % 96 == 0) {
            akk.push_back(numberToBitset(PRINT));
        }
        if (i % 96 == 94) {
            zsciiString[i].set(7, true);
        }
        akk.push_back(zsciiString[i]);
    }
    return akk;
}

std::vector<std::bitset<8>> RoutineGenerator::printReadCharInstruction(uint8_t var) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    akk.push_back(numberToBitset(READ_CHAR));
    akk.push_back(numberToBitset(0xbf));
    akk.push_back(numberToBitset(1));
    akk.push_back(numberToBitset(var));
    return akk;
}

std::vector<std::bitset<8>> RoutineGenerator::printPrintCharInstruction(uint8_t var) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    akk.push_back(numberToBitset(PRINT_CHAR));
    akk.push_back(numberToBitset(0xbf));
    akk.push_back(numberToBitset(0x10));
    return akk;
}

std::vector<std::bitset<8>> RoutineGenerator::printCallToMainAndMain(int offset, int locVar) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    size_t pkgAdrr = Utils::calculateNextPackageAddress(offset + 3);
    //call the main
    akk.push_back(bitset<8>(CALL_1N));
    Utils::setShortVal(pkgAdrr / 8, akk);
    Utils::paddingToNextPackageAddress(akk, offset);
    akk.push_back(numberToBitset(locVar));
    return akk;
}

std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number) {
    return bitset<8>(number);
}