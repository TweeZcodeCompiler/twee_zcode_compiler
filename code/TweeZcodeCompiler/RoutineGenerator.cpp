//
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>

using std::vector;
using std::bitset;

const int PRINT = 178;
const int QUIT = 186;

std::vector<std::bitset<8>> RoutineGenerator::getHelloWorldRoutine(std::string stringToPrint) {
    vector<bitset<8>> akk = vector<bitset<8>>();
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    akk.push_back(numberOfLocalVariables(0));
    akk.push_back(getOpcode(PRINT));
    for (bitset<8> bs : zsciiString) {
        for (int i = 0; i < 8; i++) {
            std::cout << bs[i];
        }
    }
    std::cout << "\n";
    akk.insert(akk.end(),zsciiString.begin(),zsciiString.end());
    akk.push_back(QUIT);
    for (bitset<8> bs : akk) {
        for (int i = 0; i < 8; i++) {
            std::cout << bs[i];
        }
    }
    return akk;
}

std::bitset<8> RoutineGenerator::numberOfLocalVariables(int number) {
    return bitset<8>(number);
}

std::bitset<8> RoutineGenerator::getOpcode(int number) {
    return bitset<8>(number);
}