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

    map<int, int> calculatedCondOffsets, calculatedUncondOffsets;

    int bla = akk.size();

    // replace jump offset placeholders
    for (map<int, string>::iterator entry = jumpToBranch.begin(); entry != jumpToBranch.end(); ++entry) {
        // throw exception if jump to unknown label
        if (branches.find(entry->second) == branches.end()) {
            cout << "Unknown label '" << entry->second << "' at address: " << entry->first << endl;
            throw;
        }

        int indexOfJumpInstruction = entry->first;                               // address where jump offset needs to be added
        int indexOfLabel = branches.find(entry->second)->second;    // destination jump address

        bitset<8> jumpInstruction = akk.find(indexOfJumpInstruction)->second;

        int offset = getOffset(indexOfJumpInstruction, indexOfLabel);



        if (jumpInstruction.test(JUMP_COND_OFFSET_PLACEHOLDER)) {
            bool jumpIfCondTrue = jumpInstruction.test(JUMP_COND_TRUE);

            offset = addCondBranchOffset(indexOfJumpInstruction, offset, jumpIfCondTrue);
            calculatedCondOffsets[indexOfJumpInstruction] = offset;
        } else {
            addLargeNumber(offset, indexOfJumpInstruction);
            calculatedUncondOffsets[indexOfJumpInstruction] = offset;
        }
    }

    bool correctOffsets = false;
    while (!correctOffsets) {
        correctOffsets = true;
        for (map<int, string>::iterator entry = jumpToBranch.begin(); entry != jumpToBranch.end(); ++entry) {
            int indexOfJumpInstruction = entry->first;                               // address where jump offset needs to be added
            int indexOfLabel = branches.find(entry->second)->second;                 // destination jump address

            int offset = getOffset(indexOfJumpInstruction, indexOfLabel);

            if (calculatedCondOffsets.find(indexOfJumpInstruction) != calculatedCondOffsets.end()) {
                int savedOffset = calculatedCondOffsets.find(indexOfJumpInstruction)->second;
                if (offset != savedOffset) {
                    bitset<8> wrongOffsetBitset = akk.find(indexOfJumpInstruction)->second;
                    addCondBranchOffset(indexOfJumpInstruction, offset,
                                                 wrongOffsetBitset.test(JUMP_COND_TRUE));

                    if ((savedOffset < 0 || savedOffset > 63) && (offset >= 0 && offset <= 63)) {
                        offset--;
                    }

                    calculatedCondOffsets[indexOfJumpInstruction] = offset;
                    correctOffsets = false;
                }
            } else {
                if (offset != calculatedUncondOffsets.find(indexOfJumpInstruction)->second) {
                    bitset<8> wrongOffsetBitset = akk.find(indexOfJumpInstruction)->second;
                    addLargeNumber(offset, indexOfJumpInstruction);
                    calculatedUncondOffsets[indexOfJumpInstruction] = offset;
                    correctOffsets = false;
                }
            }
        }
    }

    bla = akk.size();

    vector<bitset<8>> zcode;
    for (map<int, bitset<8>>::iterator entry = akk.begin(); entry != akk.end(); ++entry) {
        zcode.push_back(entry->second);
    }

    return zcode;
}

void RoutineGenerator::addBitset(std::bitset<8> byte, int pos) {
    //akk.insert(pair<int, bitset<8>>(pos < 0 ? akk.size() : pos, byte));
    akk[pos < 0 ? akk.size() : pos] = byte;
}

int RoutineGenerator::getOffset(int jumpPosition, int labelPosition) {
    int offset;
    if (jumpPosition == labelPosition) {
        offset = 0;
    } if (jumpPosition < labelPosition) {
        for (map<int, bitset<8>>::iterator it = akk.find(jumpPosition); it != akk.find(labelPosition); ++it) {
            offset++;
        }
    } else {
        for (map<int, bitset<8>>::iterator it = akk.find(jumpPosition); it != akk.find(labelPosition); --it) {
            offset--;
        }
    }
    return offset;
}

void RoutineGenerator::print(string stringToPrint) {
    ZCodeConverter converter = ZCodeConverter();
    vector<bitset<8>> zsciiString = converter.convertStringToZSCII(stringToPrint);

    unsigned long len = zsciiString.size();
    for(int i = 0; i < len; i++){
        if(i%96 == 0){
            //akk.push_back(numberToBitset(PRINT));
            addBitset(numberToBitset(PRINT));
        }
        if(i%96 == 94) {
            zsciiString[i].set(7, true);
        }
        //akk.push_back(zsciiString[i]);
        addBitset(zsciiString[i]);
    }
}

void RoutineGenerator::newLine() {
    //akk.push_back(numberToBitset(NEW_LINE));
    addBitset(numberToBitset(NEW_LINE));
}

// The destination of the jump opcode is:
// Address after instruction + Offset - 2
void RoutineGenerator::jump(string toLabel) {
    //akk.push_back(numberToBitset(JUMP));
    addBitset(numberToBitset(JUMP));
    //jumpToBranch.insert(pair<int, string>(akk.size(), toLabel));
    jumpToBranch[akk.size()] = toLabel;
    addLargeNumber(1 << (JUMP_UNCOND_OFFSET_PLACEHOLDER + 7)); // placeholder, will be replaced in getRoutine()
}

void RoutineGenerator::quitRoutine() {
    quitOpcodePrinted = true;
    //akk.push_back(numberToBitset(QUIT));
    addBitset(numberToBitset(QUIT));
}

// adds label and next instruction address to 'branches' map
void RoutineGenerator::addLabel(string label) {
    //branches.insert(pair<string, int>(label, akk.size()));
    branches[label] = akk.size();
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
    //akk.push_back(opcode);
    addBitset(opcode);

    if (oneByteParameter) {
        //akk.push_back(numberToBitset(parameter));
        addBitset(numberToBitset(parameter));
    } else {
        addLargeNumber(parameter);
    }

    //jumpToBranch.insert(pair<std::bitset<8>*, string>(&(*akk.end()), toLabel));
    //bitset<8>* branch = &akk.at(akk.size() - 1);

    //jumpToBranch.insert(pair<int, string>(akk.size(), toLabel));
    jumpToBranch[akk.size()] = toLabel;

    bitset<8> offsetFirstBits;
    offsetFirstBits.set(JUMP_COND_TRUE, jumpIfTrue);
    offsetFirstBits.set(JUMP_COND_OFFSET_PLACEHOLDER, true);
    offsetFirstBits.set(JUMP_UNCOND_OFFSET_PLACEHOLDER, false);

    //akk.push_back(offsetFirstBits); // placeholder, will be replaced in getRoutine()
    addBitset(offsetFirstBits);
    addBitset(numberToBitset(0));
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
        //akk.insert(akk.begin() + pos, firstHalf);
        //akk.insert(akk.begin() + pos + 1, secondHalf);
        addBitset(firstHalf, pos);
        addBitset(secondHalf, pos + 1);
    } else {
        //akk.push_back(firstHalf);
        //akk.push_back(secondHalf);
        addBitset(firstHalf);
        addBitset(secondHalf);
    }
}

int RoutineGenerator::addCondBranchOffset(size_t position, int16_t offset, bool jumpIfCondTrue){
    bitset<8> firstHalf, secondHalf;

    // Offset needs to be between 0 and 63 to fit into 6 bits
    bool useOneByte = offset > 0 && offset < 64;

    firstHalf.set(JUMP_COND_TRUE, jumpIfCondTrue);
    firstHalf.set(JUMP_OFFSET_5_BIT, useOneByte);

    if (useOneByte) {
        offset--;

        bitset<6> bitsetOffset (offset);

        for (size_t i = 0; i < 6; i++) {
            firstHalf.set(i, bitsetOffset[i]);
        }

        //akk.insert(akk.begin() + position, firstHalf);

        akk.erase(position + 1);
        addBitset(firstHalf, position);
    } else {
        bitset<14> bitsetOffset(offset);

        for (size_t i = 0; i < 8; i++) {
            secondHalf.set(i, bitsetOffset[i]);
        }

        for (size_t i = 8; i < 14; i++) {
            firstHalf.set(i - 8, bitsetOffset[i]);
        }

        //akk.insert(akk.begin() + position, firstHalf);
        //akk.insert(akk.begin() + position + 1, secondHalf);
        //akk.erase(position);
        //akk.erase(position+1);
        addBitset(firstHalf, position);
        addBitset(secondHalf, position + 1);
    }

    return offset;
}