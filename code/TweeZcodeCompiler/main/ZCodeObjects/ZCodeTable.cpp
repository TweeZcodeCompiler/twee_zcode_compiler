//
// Created by philip on 09.07.15.
//

#include "ZCodeTable.h"
#include "../Utils.h"

bool ZCodeTable::revalidate() {
    return false;
}

void ZCodeTable::print(std::vector<std::bitset<8>> &zcode) {
    Utils::addTwoBytes((content.size())/2,zcode);
    Utils::append(zcode,content);
}