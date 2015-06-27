//
// Created by philip on 27.06.15.
//

#include "ZCodeInstruction.h"

bool ZCodeInstruction::revalidate() {
    return false;
}

std::vector<std::bitset<8>>& ZCodeInstruction::print() {
    return content;
}