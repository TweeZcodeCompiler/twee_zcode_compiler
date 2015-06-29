//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODELABEL_H
#define PROJECT_ZCODELABEL_H


#include "ZCodeObject.h"

class ZCodeLabel: public ZCodeObject {
private:

    size_t lastOffset;
public:
    void print(std::vector<std::bitset<8>> &code);
    //returns true if needs to be revalidate again
    bool revalidate();
};


#endif //PROJECT_ZCODELABEL_H
