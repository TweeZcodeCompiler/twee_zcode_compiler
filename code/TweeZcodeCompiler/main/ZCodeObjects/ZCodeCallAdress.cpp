//
// Created by philip on 27.06.15.
//

#include "ZCodeCallAdress.h"
#include "../Utils.h"

bool ZCodeCallAdress::revalidate() {
    return false;
}

std::vector<std::bitset<8>>& ZCodeCallAdress::print() {
    uint16_t  pkgAddr = routine.offset/8;
    std::vector<std::bitset<8>> instructions;
    Utils::addTwoBytes(pkgAddr, instructions);
    return instructions;
}