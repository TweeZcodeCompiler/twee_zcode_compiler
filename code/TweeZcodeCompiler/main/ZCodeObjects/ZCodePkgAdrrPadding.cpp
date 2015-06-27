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
    std::vector<std::bitset<8>> padding = std::vector<std::bitset<8>>();
    size_t missing = Utils::paddingToNextPackageAddress(0,offset);
    Utils::fillWithBytes(code,0,missing);
}