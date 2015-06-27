//
// Created by philip on 27.06.15.
//

#include "ZCodeCallAdress.h"
#include "../Utils.h"

bool ZCodeCallAdress::revalidate() {
    return false;
}

void ZCodeCallAdress::print(std::vector<std::bitset<8>> &code) {
    uint16_t  pkgAddr = routine->offset/8;
    Utils::addTwoBytes(pkgAddr,code);
}