// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>

using std::vector;
using std::bitset;

std::vector<std::bitset<8>> RoutineGenerator::printPrintRoutine(std::string stringToPrint) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    akk.push_back(numberToBitset(0));
    akk.push_back(numberToBitset(PRINT));
    akk.insert(akk.end(),zsciiString.begin(),zsciiString.end());
    akk.push_back(numberToBitset(QUIT));
    return akk;
}

std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number){
    return bitset<8>(number);
}