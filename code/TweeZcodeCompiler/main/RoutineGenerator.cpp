// Implementation of RoutineGenerator.cpp
// Created by philip on 10.05.15.
//

#include "RoutineGenerator.h"
#include "ZCodeConverter.h"
#include <iostream>
#include <algorithm>
#include <list>

using namespace std;

std::vector<std::bitset<8>> RoutineGenerator::getRoutine() {
    if (!quitOpcodePrinted) {
        cout << "Routine has no quit expression!" << endl;
        throw;
    }


    std::list<int> ilist;
    ilist.push_back(1);
    ilist.push_back(2);
    ilist.push_back(3);
    ilist.push_back(1);

    list<int>::iterator findIter = std::find(ilist.begin(), ilist.end(), 1);
    cout << "Found: " << &(*findIter) << endl;


    cout << "Vorher: " << akk.at(3) << endl;

    bitset<8>* t = &akk.at(3);
    cout << "Adresse: " << t << endl;

    t->reset();
    t->set(0, true);

    cout << "Nachher: " << akk.at(3) << endl;

    akk.erase(akk.begin() + 2);
    cout << "Adresse 2: " << &akk.at(2) << ", " << akk.at(2) << endl;
    cout << "Adresse 3: " << &akk.at(3) << ", " << akk.at(3) << endl;

    // replace jump offset placeholders
    for (map<bitset<8>*, string>::iterator entry = jumpToBranch.begin(); entry != jumpToBranch.end(); ++entry) {
        // throw exception if jump to unknown label
        if (branches.find(entry->second) == branches.end()) {
            cout << "Unknown label '" << entry->second << "' at address: " << entry->first << endl;
            throw;
        }

        bitset<8>* jumpInstruction = entry->first;                             // address where jump offset needs to be added
        bitset<8>* labeledInstruction = branches.find(entry->second)->second;    // destination jump address

        //bitset<8> offsetPlaceholder = akk.at(jumpInstruction - firstInstructionAddress);
        //int offsetPos = jumpInstruction - firstInstructionAddress;

        vector<bitset<8>>::iterator it = find(akk.begin(), akk.end(), *jumpInstruction);
        int indexOfJumpInstruction = -1;  //it - akk.begin(); //labeledInstruction - jumpInstruction;
        //it = find(akk.begin(), akk.end(), *labeledInstruction);
        int indexOfLabel = -1; //it - akk.begin();

        bool jumpFound = false, labelFound = false;
        for(vector<bitset<8>>::iterator it = akk.begin(); it != akk.end(); ++it) {
            bitset<8>* instP = &(*it);

            if (!jumpFound) indexOfJumpInstruction++;
            if (!labelFound) indexOfLabel++;

            if(instP == jumpInstruction) {
                jumpFound = true;
            } else if (instP == labeledInstruction) {
                labelFound = true;
            }

            if (jumpFound && labelFound) {
                break;
            }
        }

        int offset = indexOfLabel - indexOfJumpInstruction;

        if (jumpInstruction->test(JUMP_COND_OFFSET_PLACEHOLDER)) {
            bool jumpIfCondTrue = jumpInstruction->test(JUMP_COND_TRUE);

            akk.erase(akk.begin() + indexOfJumpInstruction);
            addCondBranchOffset(indexOfJumpInstruction, offset, jumpIfCondTrue);
        } else {
            akk.erase(akk.begin() + indexOfJumpInstruction);
            akk.erase(akk.begin() + indexOfJumpInstruction);
            addLargeNumber(offset, indexOfJumpInstruction);
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
    jumpToBranch.insert(pair<std::bitset<8>*, string>(&akk.back(), toLabel));
    addLargeNumber(1 << (JUMP_UNCOND_OFFSET_PLACEHOLDER + 7)); // placeholder, will be replaced in getRoutine()
}

void RoutineGenerator::quitRoutine() {
    quitOpcodePrinted = true;
    akk.push_back(numberToBitset(QUIT));
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::addLabel(string label) {
    int test = akk.size();

    vector<bitset<8>>::iterator it = akk.end();
    bitset<8>* instruction = &(*it);
    branches.insert(pair<string, std::bitset<8>*>(label, instruction));
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

    //jumpToBranch.insert(pair<std::bitset<8>*, string>(&(*akk.end()), toLabel));
    int test = akk.size();
    bitset<8>* branch = &akk.at(akk.size() - 1);
    jumpToBranch.insert(pair<bitset<8>*, string>(branch, toLabel));

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