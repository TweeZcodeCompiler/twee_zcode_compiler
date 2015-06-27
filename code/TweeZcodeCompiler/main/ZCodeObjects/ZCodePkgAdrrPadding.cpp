//
// Created by philip on 27.06.15.
//

#include "ZCodePkgAdrrPadding.h"
#include "../Utils.h"

bool ZCodePkgAdrrPadding::revalidate() {
    size = Utils::calculateNextPackageAddress(offset) - this->offset;
    return false;
}

std::vector<std::bitset<8>>& ZCodePkgAdrrPadding::print() {
    std::vector<std::bitset<8>> padding = std::vector<std::bitset<8>>();
    size_t missing = Utils::paddingToNextPackageAddress(0,offset);
    Utils::fillWithBytes(padding,0,missing);
    return padding;
}