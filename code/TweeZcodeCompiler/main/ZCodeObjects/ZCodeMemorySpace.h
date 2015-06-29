//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEMEMORYSPACE_H
#define PROJECT_ZCODEMEMORYSPACE_H


#include "ZCodeObject.h"
#include "../Utils.h"

class ZCodeMemorySpace : public ZCodeObject{
private:
    std::vector<std::bitset<8>> space;
public:
    std::vector<std::bitset<8>> &print();
    bool revalidate();
    ZCodeMemorySpace(int space){
        Utils::fillWithBytes(this->space,0,space);
        this->size = space;
    }

};


#endif //PROJECT_ZCODEMEMORYSPACE_H
