//
// Created by philip on 27.06.15.
//

#include "ZCodeLabel.h"

void ZCodeLabel::print(std::vector<std::bitset<8>> &code) {
    return ;
}

bool ZCodeLabel::revalidate() {
    if(lastOffset != offset){
        lastOffset = offset;
        return true;
    }
    return false;
}