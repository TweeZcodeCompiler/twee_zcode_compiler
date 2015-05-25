// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>

using std::vector;
using std::bitset;

std::vector<std::bitset<8>> RoutineGenerator::getRoutine() {
    return akk;
}

void RoutineGenerator::printPrintRoutine(std::string stringToPrint) {
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    unsigned long len = zsciiString.size();
    for(int i = 0; i < len; i++){
        if(i%96 == 0){
            akk.push_back(numberToBitset(PRINT));
        }
        if(i%96 == 94) {
            zsciiString[i].set(7, true);
        }
        akk.push_back(zsciiString[i]);
    }
}

void RoutineGenerator::newLine() {
    akk.push_back(numberToBitset(NEW_LINE));
}

void RoutineGenerator::quitRoutine() {
    akk.push_back(numberToBitset(QUIT));
}

std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number){
    return bitset<8>(number);
}