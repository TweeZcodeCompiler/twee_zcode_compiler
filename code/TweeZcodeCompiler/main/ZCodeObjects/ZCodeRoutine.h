//
// Created by philip on 27.06.15.
//

#ifndef PROJECT_ZCODEROUTINE_H
#define PROJECT_ZCODEROUTINE_H


#include "ZCodeContainer.h"
#include "ZCodeInstruction.h"
#include <map>

class ZCodeRoutine: public ZCodeContainer {
private:
    static std::map<std::string, &ZCodeRoutine> routines;
    uint8_t localVariables;
public:
    std::vector<std::bitset<8>> &print();
    ZCodeRoutine(uint8_t locVariables){
       this->localVariables = locVariables;
    }

    static ZCodeRoutine &GetOrCreateRoutine(std::string name, uint8_t locVariables);

};


#endif //PROJECT_ZCODEROUTINE_H
