//
// Created by philip on 07.06.15.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

#include <bitset>
#include <vector>
#include <cstdint>
#include <map>

class Utils {

public:
    // splits 16 bit value up to 2 bytes and pushes them into the bitvector
    static void addTwoBytes(uint16_t val, std::vector<std::bitset<8>> &bitset);

    static void fillWithBytes(std::vector<std::bitset<8>> &bitVector, uint8_t value, unsigned long amountOfBytes);

    static size_t calculateNextPackageAddress(size_t currentOffset);

    static size_t paddingToNextPackageAddress(size_t vector_size, size_t offset);

    static void append(std::vector<std::bitset<8>> &head, std::vector<std::bitset<8>> &tail);

    static void insertPaddingToNextRoutine(std::vector<std::bitset<8>> &bitsets, size_t routineOffset);

    static int includes(std::vector<std::string>, std::string);
};


#endif //PROJECT_UTILS_H
