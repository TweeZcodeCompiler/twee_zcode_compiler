//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEINSTRUCTION_H
#define PROJECT_ZCODEINSTRUCTION_H


#include <bitset>
#include <vector>
#include "ZCodeObject.h"

class ZCodeInstruction : public ZCodeObject {
private:
    std::vector<std::bitset<8>> content;
public:
    //returns true if needs to be revalidate again
    bool revalidate();
    std::vector<std::bitset<8>>& print();

    ZCodeInstruction(std::vector<std::bitset<8>> instructions) {
        content = instructions;
        this->size = instructions.size();
    }
};


#endif //PROJECT_ZCODEINSTRUCTION_H