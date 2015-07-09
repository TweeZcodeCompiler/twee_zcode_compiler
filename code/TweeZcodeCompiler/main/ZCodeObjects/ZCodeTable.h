//
// Created by philip on 09.07.15.
//

#ifndef PROJECT_ZCODETABLE_H
#define PROJECT_ZCODETABLE_H


#include "ZCodeObject.h"

class ZCodeTable : public ZCodeObject{

private:
    std::vector<std::bitset<8>> content;
public:
    bool revalidate();
    void print(std::vector<std::bitset<8>> &zcode);
    ZCodeTable(std::vector<std::bitset<8>> content): content(content){
        size = content.size()+2;
    }

};


#endif //PROJECT_ZCODETABLE_H
