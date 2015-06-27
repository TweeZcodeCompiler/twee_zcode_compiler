//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODELABEL_H
#define PROJECT_ZCODELABEL_H


#include "ZCodeObject.h"

class ZCodeLabel: public ZCodeObject {
public:
    virtual std::vector<std::bitset<8>> &print();
    //returns true if needs to be revalidate again
    virtual bool revalidate();

};


#endif //PROJECT_ZCODELABEL_H
