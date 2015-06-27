//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEJUMP_H
#define PROJECT_ZCODEJUMP_H


#include "ZCodeObject.h"
#include "ZCodeLabel.h"

class ZCodeJump : public ZCodeObject {
private:
    size_t lastOffset = 0;
    ZCodeLabel label;
    std::vector<std::bitset<8>> adress;
    void addCondBranchOffset();
public:
    bool jumpIfCondTrue = true;
    std::vector<std::bitset<8>> &print();
    bool revalidate();
    ZCodeJump(ZCodeLabel &label){
        this->label = label;
    }
};


#endif //PROJECT_ZCODEJUMP_H