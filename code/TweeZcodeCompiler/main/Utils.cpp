//
// Created by philip on 07.06.15.
//

#include "Utils.h"

using std::bitset;
using std::vector;

// splits short value up to 2 bytes and pushes them into the headerBits
void Utils::setShortVal(unsigned short val, vector<bitset<8>> &bitvector) {
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

void Utils::paddingToNextPackageAddress(std::vector<std::bitset<8>> &bitset, size_t offset) {
    size_t pkgAdrr = Utils::calculateNextPackageAddress(offset + 3);
    size_t empty = pkgAdrr - bitset.size() - offset;
    Utils::fillWithBytes(bitset, 0, (empty > 0) ? empty : 0);
}

void Utils::append(std::vector<std::bitset<8>> &head, std::vector<std::bitset<8>> &tail) {
    head.insert(head.end(), tail.begin(), tail.end());
}