//
// Created by philip on 07.06.15.
//

#include <stdint-gcc.h>
#include "Utils.h"

using std::bitset;
using std::vector;

void Utils::addTwoBytes(uint16_t val, vector<bitset<8>> &bitvector) {
    bitset<16> shortVal(val);
    bitset<8> firstHalf, secondHalf;

    for (size_t i = 0; i < 8; i++) {
        secondHalf.set(i, shortVal[i]);
    }

    for (size_t i = 8; i < 16; i++) {
        firstHalf.set(i - 8, shortVal[i]);
    }

    bitvector.push_back(firstHalf);
    bitvector.push_back(secondHalf);
}

void Utils::fillWithBytes(std::vector<std::bitset<8>> &bitVector, uint8_t value, unsigned long amountOfBytes) {
    for (size_t i = 0; i < amountOfBytes; i++) {
        bitVector.insert(bitVector.end(), value);
    }
}

size_t Utils::calculateNextPackageAddress(size_t currentOffset) {
    return ((currentOffset + 8) / 8) * 8;
}

size_t Utils::paddingToNextPackageAddress(size_t vector_size, size_t offset) {
    size_t pkgAdrr = Utils::calculateNextPackageAddress(offset + vector_size + 3);
    size_t empty = (pkgAdrr - vector_size - offset) % 8;
    return (empty > 0) ? empty : 0;
}

void Utils::append(std::vector<std::bitset<8>> &head, std::vector<std::bitset<8>> &tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}

void Utils::insertPaddingToNextRoutine(vector<bitset<8>> &bitsets, size_t routineOffset) {
    size_t padding = Utils::paddingToNextPackageAddress(bitsets.size(), routineOffset);

    for (size_t i = 0; i < padding; i++) {
        bitsets.push_back(bitset<8>(0));
    }
}