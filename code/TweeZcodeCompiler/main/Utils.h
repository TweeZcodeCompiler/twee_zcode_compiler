//
// Created by philip on 07.06.15.
//

#ifndef PROJECT_UTILS_H
#define PROJECT_UTILS_H

#include <bitset>
#include <vector>
#include <cstdint>

class Utils {

public:
    // split short values upt to 2 bytes
    static void setShortVal(uint16_t val, std::vector<std::bitset<8>> &bitset);
    static void fillWithBytes(std::vector<std::bitset<8>>& bitVector, uint8_t value, unsigned long amountOfBytes);
    static size_t calculateNextPackageAddress(size_t currentOffset);
    static void paddingToNextPackageAddress(std::vector<std::bitset<8>> &bitset, size_t offset);
    static void append(std::vector<std::bitset<8>> &head, std::vector<std::bitset<8>> &tail);
};


#endif //PROJECT_UTILS_H
