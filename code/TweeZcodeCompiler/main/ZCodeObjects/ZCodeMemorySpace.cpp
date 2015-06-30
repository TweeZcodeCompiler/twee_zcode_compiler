//
// Created by philip on 27.06.15.
//

#include "ZCodeMemorySpace.h"

void ZCodeMemorySpace::print(std::vector<std::bitset<8>> &code) {
    Utils::fillWithBytes(code,0,this->getSize());
}

bool ZCodeMemorySpace::revalidate() {
    return false;
}