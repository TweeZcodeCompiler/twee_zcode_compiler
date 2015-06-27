//
// Created by philip on 27.06.15.
//

#include "ZCodeInstruction.h"
#include "../Utils.h"

bool ZCodeInstruction::revalidate() {
    return false;
}

void ZCodeInstruction::print(std::vector<std::bitset<8>> &code) {
    Utils::append(code,content);
}