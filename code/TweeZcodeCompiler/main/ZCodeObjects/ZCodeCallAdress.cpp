//
// Created by philip on 27.06.15.
//

#include "ZCodeCallAdress.h"
#include "../Utils.h"

bool ZCodeCallAdress::revalidate() {
    return false;
}

void ZCodeCallAdress::print(std::vector<std::bitset<8>> &code) {
    uint16_t  pkgAddr = (isPackaged)?routine->offset/8:4;
    displayName = "call adress "+ std::to_string(routine->offset);
    Utils::addTwoBytes(pkgAddr,code);
}