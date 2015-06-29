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
    std::shared_ptr<ZCodeLabel> label;
    std::vector<std::bitset<8>> adress;
    void addCondBranchOffset();
public:
    bool isCondJump = true;
    bool jumpIfCondTrue = true;
     void print(std::vector<std::bitset<8>> &code);
    virtual bool revalidate();

    ZCodeJump(std::shared_ptr<ZCodeLabel>label): label(label){
    }
};


#endif //PROJECT_ZCODEJUMP_H
