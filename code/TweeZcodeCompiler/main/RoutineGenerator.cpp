// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>

using namespace std;

std::vector<std::bitset<8>> RoutineGenerator::getRoutine() {
    if (!quitOpcodePrinted) {
        cout << "Routine has no quit expression!" << endl;
        throw;
    }

    // replace jump offset placeholders
    for (map<u_int, string>::iterator entry = jumpToBranch.begin(); entry != jumpToBranch.end(); ++entry) {
        // throw exception if jump to unknown label
        if (branches.find(entry->second) == branches.end()) {
            cout << "Unknown label '" << entry->second << "' at address: " << entry->first << endl;
            throw;
        }

        u_int instructionAdress = entry->first;                             // address where jump offset needs to be added
        u_int destinationAddress = branches.find(entry->second)->second;    // destination jump address

        akk.erase(akk.begin() + (instructionAdress - firstInstructionAddress));
        akk.erase(akk.begin() + (instructionAdress - firstInstructionAddress));
        addLargeNumber(destinationAddress - instructionAdress, instructionAdress - firstInstructionAddress);
    }

    return akk;
}

void RoutineGenerator::printPrintRoutine(string stringToPrint) {
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

//The destination of the jump opcode is:
//Address after instruction + Offset - 2
void RoutineGenerator::jump(string toLabel) {
    akk.push_back(numberToBitset(JUMP));
    jumpToBranch.insert(pair<int, string>(firstInstructionAddress + akk.size(), toLabel));
    addLargeNumber(-1); // placeholder, will be replaced in getRoutine()
}

void RoutineGenerator::quitRoutine() {
    quitOpcodePrinted = true;
    akk.push_back(numberToBitset(QUIT));
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::addBranch(string label) {
    branches.insert(pair<string, int>(label, firstInstructionAddress + akk.size()));
}


std::bitset<8> RoutineGenerator::numberToBitset(unsigned int number){
    return bitset<8>(number);
}

void RoutineGenerator::addLargeNumber(int16_t number) {
    addLargeNumber(number, -1);
}

void RoutineGenerator::addLargeNumber(int16_t number, int pos) {
    bitset<16> shortVal (number);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    if (pos >= 0) {
        akk.insert(akk.begin() + pos, firstHalf);
        akk.insert(akk.begin() + pos + 1, secondHalf);
    } else {
        akk.push_back(firstHalf);
        akk.push_back(secondHalf);
    }
}