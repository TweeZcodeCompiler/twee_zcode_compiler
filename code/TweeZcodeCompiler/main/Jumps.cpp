//
// Created by tobias on 6/7/15.
//

#include "Jumps.h"
#include "RoutineGenerator.h"
#include <iostream>

using namespace std;

void Jumps::newJump(string toLabel) {
    jumpToBranch[routineOpcodes->size()] = toLabel;
}

void Jumps::newLabel(string label) {
    branches[label] = routineOpcodes->size();
}

void Jumps::calculateOffsets() {
    map<int, int> calculatedCondOffsets, calculatedUncondOffsets;

    // replace jump offset placeholders
    for (map<int, string>::iterator entry = jumpToBranch.begin(); entry != jumpToBranch.end(); ++entry) {
        // throw exception if jump to unknown label
        if (branches.find(entry->second) == branches.end()) {
            cout << "Unknown label '" << entry->second << "' at address: " << entry->first << endl;
            throw;
        }

        int indexOfJumpInstruction = entry->first;                  // address where jump offset needs to be added
        int indexOfLabel = branches.find(entry->second)->second;    // destination jump address

        bitset<8> jumpInstruction = routineOpcodes->find(indexOfJumpInstruction)->second;

        int offset = getOffset(indexOfJumpInstruction, indexOfLabel);

        if (jumpInstruction.test(RoutineGenerator::JUMP_COND_OFFSET_PLACEHOLDER)) {
            bool jumpIfCondTrue = jumpInstruction.test(RoutineGenerator::JUMP_COND_TRUE);

            offset = addCondBranchOffset(indexOfJumpInstruction, offset, jumpIfCondTrue);
            if (offset > 0 && (offset - 1) < 64) {
                calculateCallOffsets(indexOfJumpInstruction);
            }
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
                    bitset<8> wrongOffsetBitset = routineOpcodes->find(indexOfJumpInstruction)->second;
                    addCondBranchOffset(indexOfJumpInstruction, offset,
                                        wrongOffsetBitset.test(RoutineGenerator::JUMP_COND_TRUE));

                    if ((savedOffset < 0 || savedOffset > 63) && (offset >= 0 && offset <= 63)) {
                        routineOpcodes->erase(indexOfJumpInstruction + 1);
                    }

                    calculatedCondOffsets[indexOfJumpInstruction] = offset;
                    correctOffsets = false;
                }
            } else {
                if (offset != calculatedUncondOffsets.find(indexOfJumpInstruction)->second) {
                    addLargeNumber(offset, indexOfJumpInstruction);
                    calculatedUncondOffsets[indexOfJumpInstruction] = offset;
                    correctOffsets = false;
                }
            }
        }
    }
}

void Jumps::setRoutineBitsetMap(map<int, bitset<8>> &opcodes) {
    routineOpcodes = &opcodes;
}

int Jumps::getOffset(int jumpPosition, int labelPosition) {
    int offset = 0;

    if (jumpPosition < labelPosition) {
        for (map<int, bitset<8>>::iterator it = routineOpcodes->find(jumpPosition);
             it != routineOpcodes->find(labelPosition); ++it) {
            offset++;
        }
    } else if (jumpPosition > labelPosition) {
        for (map<int, bitset<8>>::iterator it = routineOpcodes->find(jumpPosition);
             it != routineOpcodes->find(labelPosition); --it) {
            offset--;
        }
    }

    if (offset >= 0 && offset <= 63) {
        offset++;
    }

    return offset;
}

int Jumps::addCondBranchOffset(size_t position, int16_t offset, bool jumpIfCondTrue) {
    bitset<8> firstHalf, secondHalf;

    // Offset needs to be between 0 and 63 to fit into 6 bits
    bool useOneByte = offset > 0 && (offset - 1) < 64;

    firstHalf.set(RoutineGenerator::JUMP_COND_TRUE, jumpIfCondTrue);
    firstHalf.set(RoutineGenerator::JUMP_OFFSET_5_BIT, useOneByte);

    if (useOneByte) {
        bitset<6> bitsetOffset(offset);

        for (size_t i = 0; i < 6; i++) {
            firstHalf.set(i, bitsetOffset[i]);
        }

        routineOpcodes->erase(position + 1);
        addBitset(firstHalf, position);
    } else {
        bitset<14> bitsetOffset(offset);

        for (size_t i = 0; i < 8; i++) {
            secondHalf.set(i, bitsetOffset[i]);
        }

        for (size_t i = 8; i < 14; i++) {
            firstHalf.set(i - 8, bitsetOffset[i]);
        }

        addBitset(firstHalf, position);
        addBitset(secondHalf, position + 1);
    }

    return offset;
}

void Jumps::addLargeNumber(int16_t number, int pos) {
    bitset<16> shortVal(number);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    if (pos >= 0) {
        addBitset(firstHalf, pos);
        addBitset(secondHalf, pos + 1);
    } else {
        addBitset(firstHalf);
        addBitset(secondHalf);
    }
}

void Jumps::addBitset(std::bitset<8> byte, int pos) {
    (*routineOpcodes)[pos < 0 ? routineOpcodes->size() : pos] = byte;
}

void Jumps::calculateCallOffsets(size_t position) {
    vector<size_t> offsets = vector<size_t>();
    typedef map<size_t, string>::iterator it_type;
    for (it_type it = RoutineGenerator::callTo.begin(); it != RoutineGenerator::callTo.end(); it++) {

        if (it->first > position + routineOffset) {
            offsets.push_back(it->first);
        }
    }

    for (int i = 0; i < offsets.size(); i++) {
        RoutineGenerator::callTo[offsets[i] - 1] = RoutineGenerator::callTo[offsets[i]];
        RoutineGenerator::callTo.erase(offsets[i]);
        std::cout << (offsets[i] - 1) << "\n";
    }
}
