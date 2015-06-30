//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEMEMORYSPACE_H
#define PROJECT_ZCODEMEMORYSPACE_H


#include "ZCodeObject.h"
#include "../Utils.h"

class ZCodeMemorySpace : public ZCodeObject {
public:
    void print(std::vector<std::bitset<8>> &code);

    bool revalidate();

    ZCodeMemorySpace(int space) {
        setSize(space);
    }

    ZCodeMemorySpace(int space, std::string name) {
        displayName = name;
        setSize(space);
    }
};


#endif //PROJECT_ZCODEMEMORYSPACE_H
