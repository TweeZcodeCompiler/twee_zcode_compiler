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
    void print(std::vector<std::bitset<8>> &code);

    ZCodeInstruction(std::vector<std::bitset<8>> instructions,std::string name = "UNKNOWN") : ZCodeObject(name) {
        content = instructions;
        setSize(instructions.size());
    }

    ZCodeInstruction(uint8_t code, std::string name = "UNKNOWN") : ZCodeObject(name) {
        content = std::vector<std::bitset<8>>();
        content.push_back(std::bitset<8>(code));
        size = 1;
    }
};


#endif //PROJECT_ZCODEINSTRUCTION_H
