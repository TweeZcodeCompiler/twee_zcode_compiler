//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODECONTAINER_H
#define PROJECT_ZCODECONTAINER_H


#include "ZCodeObject.h"

class ZCodeContainer: public ZCodeObject {

public:
    std::vector<std::bitset<8>> &print();
    bool revalidate();

};


#endif //PROJECT_ZCODECONTAINER_H
