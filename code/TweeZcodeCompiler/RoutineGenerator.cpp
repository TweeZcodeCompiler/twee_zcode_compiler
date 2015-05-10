// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>

using std::vector;
using std::bitset;

//Opcode for print operation; following by Z-character String
const int PRINT = 178;
//Opcode: qzit the main; no arguments.
const int QUIT = 186;

std::vector<std::bitset<8>> RoutineGenerator::getHelloWorldRoutine(std::string stringToPrint) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    akk.push_back(numberOfLocalVariables(0));
    akk.push_back(getOpcode(PRINT));
    akk.insert(akk.end(),zsciiString.begin(),zsciiString.end());
    akk.push_back(getOpcode(QUIT));
    return akk;
}

std::bitset<8> RoutineGenerator::numberOfLocalVariables(int number) {
    return bitset<8>(number);
}

std::bitset<8> RoutineGenerator::getOpcode(int number) {
    return bitset<8>(number);
}