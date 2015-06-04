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

        bitset<8> offsetPlaceholder = akk.at(instructionAdress - firstInstructionAddress);
        int offsetPos = instructionAdress - firstInstructionAddress;
        int offset = destinationAddress - instructionAdress;
        if (offsetPlaceholder.test(JUMP_COND_OFFSET_PLACEHOLDER)) {
            bool jumpIfCondTrue = offsetPlaceholder.test(JUMP_COND_TRUE);

            akk.erase(akk.begin() + (instructionAdress - firstInstructionAddress));
            addCondBranchOffset(offsetPos, offset, jumpIfCondTrue);
        } else {
            akk.erase(akk.begin() + offsetPos);
            akk.erase(akk.begin() + offsetPos);
            addLargeNumber(offset, offsetPos);
        }
    }

    return akk;
}

void RoutineGenerator::print(string stringToPrint) {
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

// The destination of the jump opcode is:
// Address after instruction + Offset - 2
void RoutineGenerator::jump(string toLabel) {
    akk.push_back(numberToBitset(JUMP));
    jumpToBranch.insert(pair<int, string>(firstInstructionAddress + akk.size(), toLabel));
    addLargeNumber(1 << (JUMP_UNCOND_OFFSET_PLACEHOLDER + 7)); // placeholder, will be replaced in getRoutine()
}

void RoutineGenerator::quitRoutine() {
    quitOpcodePrinted = true;
    akk.push_back(numberToBitset(QUIT));
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::addBranch(string label) {
    branches.insert(pair<string, int>(label, firstInstructionAddress + akk.size()));
}

void RoutineGenerator::jumpZero(int16_t parameter, bool parameterIsVariable, std::string toLabel, bool jumpIfTrue) {
    bitset<8> opcode = numberToBitset(JZ);
    bool oneByteParameter = true;

    if (parameterIsVariable) {
        opcode.set(TYPE_VARIABLE, parameterIsVariable);
    } else {
        if (parameter < 0) {
            cout << "Constant < 0 not allowed at address: " << firstInstructionAddress + akk.size() << endl;
            throw;
        } else if (parameter < 256) {
            opcode.set(TYPE_SMALL_CONSTANT, true);
        } else {
            oneByteParameter = false;
        }
    }
    akk.push_back(opcode);

    if (oneByteParameter) {
        akk.push_back(numberToBitset(parameter));
    } else {
        addLargeNumber(parameter);
    }

    jumpToBranch.insert(pair<int, string>(firstInstructionAddress + akk.size(), toLabel));

    bitset<8> offsetFirstBits;
    offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
    offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
    offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

    akk.push_back(offsetFirstBits); // placeholder, will be replaced in getRoutine()
}

/*void RoutineGenerator::jumpZeroConstant(u_int16_t constant, string toLabel, bool jumpIfTrue) {
    bitset<8> opcode = numberToBitset(JZ);
    bool oneByteOperand = false;
    bitset<8> firstHalfOperand, secondHalfOperand;

    if (constant < 256) {
        opcode.set(TYPE_SMALL_CONSTANT, true);
        oneByteOperand = true;
        firstHalfOperand = numberToBitset(constant);
    } else {
        bitset<16> largeConstant  (constant);

        for (size_t i = 0; i < 8; i++) {
            secondHalfOperand.set(i, largeConstant[i]);
        }

        for (size_t i = 8; i < 16; i++) {
            firstHalfOperand.set(i - 8, largeConstant[i]);
        }
    }
    akk.push_back(opcode);

    akk.push_back(firstHalfOperand);
    if (!oneByteOperand) {
        akk.push_back(secondHalfOperand);
    }

    jumpToBranch.insert(pair<int, string>(firstInstructionAddress + akk.size(), toLabel));

    bitset<8> offsetFirstBits;
    offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
    offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
    offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

    akk.push_back(offsetFirstBits); // placeholder, will be replaced in getRoutine()
}*/


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

void RoutineGenerator::addCondBranchOffset(size_t position, int16_t offset, bool jumpIfCondTrue){
    bitset<8> firstHalf, secondHalf;

    if (offset > 0) {
        offset += 1;
    }

    // Offset needs to be between 0 and 63 to fit into 6 bits
    bool useOneByte = offset > 0 && offset < 64;

    firstHalf.set(JUMP_COND_TRUE, jumpIfCondTrue);
    firstHalf.set(JUMP_OFFSET_5_BIT, useOneByte);

    if (useOneByte) {
        bitset<6> bitsetOffset (offset);

        for (size_t i = 0; i < 6; i++) {
            firstHalf.set(i, bitsetOffset[i]);
        }

        akk.insert(akk.begin() + position, firstHalf);
    } else {
        bitset<14> bitsetOffset(offset);

        for (size_t i = 0; i < 8; i++) {
            secondHalf.set(i, bitsetOffset[i]);
        }

        for (size_t i = 8; i < 14; i++) {
            firstHalf.set(i - 8, bitsetOffset[i]);
        }

        akk.insert(akk.begin() + position, firstHalf);
        akk.insert(akk.begin() + position + 1, secondHalf);
    }
}