//
// Created by philip on 27.06.15.
//

#include "ZCodeJump.h"

void ZCodeJump::addCondBranchOffset(size_t position, int16_t offset, bool jumpIfCondTrue) {
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
}