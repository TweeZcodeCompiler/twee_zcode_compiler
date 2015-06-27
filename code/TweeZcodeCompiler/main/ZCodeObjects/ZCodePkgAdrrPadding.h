//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEPKGADRRPADDING_H
#define PROJECT_ZCODEPKGADRRPADDING_H


#include "ZCodeObject.h"

class ZCodePkgAdrrPadding : public ZCodeObject{

public:
    std::vector<std::bitset<8>> &print();
    bool revalidate();

};


#endif //PROJECT_ZCODEPKGADRRPADDING_H
