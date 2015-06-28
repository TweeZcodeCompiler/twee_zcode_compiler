//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODECONTAINER_H
#define PROJECT_ZCODECONTAINER_H


#include "ZCodeObject.h"

class ZCodeContainer: public ZCodeObject {

public:
    size_t containerOffset = 0;
    ZCodeContainer() {}
    ZCodeContainer(std::string name): ZCodeObject(name){

    }

    void print(std::vector<std::bitset<8>> &code);
    bool revalidate();

};


#endif //PROJECT_ZCODECONTAINER_H
