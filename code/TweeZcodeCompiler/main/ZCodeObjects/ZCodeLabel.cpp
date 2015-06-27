//
// Created by philip on 27.06.15.
//

#include "ZCodeLabel.h"

std::vector<std::bitset<8>>& ZCodeLabel::print() {
    std::vector<std::bitset<8>> none;
    return none;
}

bool ZCodeLabel::revalidate() {
    if(lastOffset != offset){
        lastOffset = offset;
        return true;
    }
    return false;
}