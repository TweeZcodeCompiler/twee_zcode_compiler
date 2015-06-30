//
// Created by philip on 27.06.15.
//

#include "ZCodePkgAdrrPadding.h"
#include "../Utils.h"

bool ZCodePkgAdrrPadding::revalidate() {
    size = Utils::calculateNextPackageAddress(offset) - this->offset;
    return false;
}

void ZCodePkgAdrrPadding::print(std::vector<std::bitset<8>> &code) {
    Utils::fillWithBytes(code,0,size);
}