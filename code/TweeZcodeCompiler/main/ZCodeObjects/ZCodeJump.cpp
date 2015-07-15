//
// Created by philip on 27.06.15.
//

#include "ZCodeJump.h"
#include "../RoutineGenerator.h"

using std::vector;
using std::bitset;

void ZCodeJump::print(std::vector<std::bitset<8>> &code) {
    Utils::append(code,this->adress);
}

bool ZCodeJump::revalidate() {
    this->displayName = "jump adress "+std::to_string((int)this->label->offset);
    int size = this->adress.size();
    addCondBranchOffset();
    if(size != this->adress.size()){
        return true;
    }
    if(lastOffset != offset){
        lastOffset = offset;
        return true;
    }
    return false;
}

void ZCodeJump::addCondBranchOffset() {

    bitset<8> firstHalf, secondHalf;
    this->adress = vector<bitset<8>>();
    int offset = label->offset - this->offset;
    if(offset > 0){
        offset += 1;
    }
    bool useOneByte = false;
    if(!isCondJump){
        if (offset > 0) offset--;

        bitset<16> bitsetOffset(offset);

        for (size_t i = 0; i < 8; i++) {
            secondHalf.set(i, bitsetOffset[i]);
        }

        for (size_t i = 8; i < 16; i++) {
            firstHalf.set(i - 8, bitsetOffset[i]);
        }
        this->adress.push_back(firstHalf);
        this->adress.push_back(secondHalf);
        this->size = 2;
        return;
    }

    // Offset needs to be between 0 and 63 to fit into 6 bits
    useOneByte = offset > 0 && (offset - 1) < 64;

    firstHalf.set(RoutineGenerator::JUMP_COND_TRUE, jumpIfCondTrue);
    firstHalf.set(RoutineGenerator::JUMP_OFFSET_5_BIT, useOneByte);

    if (useOneByte) {
        bitset<6> bitsetOffset(offset);

        for (size_t i = 0; i < 6; i++) {
            firstHalf.set(i, bitsetOffset[i]);
        }
        this->adress.push_back(firstHalf);
        this->size = 1;
    } else {
        if (offset > 0) offset--;
        bitset<14> bitsetOffset(offset);

        for (size_t i = 0; i < 8; i++) {
            secondHalf.set(i, bitsetOffset[i]);
        }

        for (size_t i = 8; i < 14; i++) {
            firstHalf.set(i - 8, bitsetOffset[i]);
        }
        this->adress.push_back(firstHalf);
        this->adress.push_back(secondHalf);
        this->size = 2;
    }
}