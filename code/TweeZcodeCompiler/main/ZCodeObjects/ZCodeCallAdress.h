//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODECALLADRESS_H
#define PROJECT_ZCODECALLADRESS_H


#include "ZCodeObject.h"
#include "ZCodeRoutine.h"

class ZCodeCallAdress:public ZCodeObject {
private:
    std::shared_ptr<ZCodeRoutine> routine;
public:
    bool revalidate();
    void print(std::vector<std::bitset<8>> &code);
    ZCodeCallAdress(std::shared_ptr<ZCodeRoutine> routine) : routine(routine) {
        setSize(2);
    }
};


#endif //PROJECT_ZCODECALLADRESS_H
