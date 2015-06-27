//
// Created by philip on 27.06.15.
//

#include "ZCodeRoutine.h"
#include "../Utils.h"

ZCodeRoutine &ZCodeRoutine::GetOrCreateRoutine(std::string name, std::uint8_t locVariables) {
    std::map<std::string, ZCodeRoutine>::iterator p;
    p = routines.find(name);
    if (p == routines.end()) {
        routines.insert(pair<std::string, ZCodeRoutine>(name, ZCodeRoutine(0)));
    } else {
        ZCodeRoutine routine = p->second;
        if(routine.localVariables < locVariables){
            routine.localVariables = locVariables;
        }
        return routine;
    }
}

std::vector<std::bitset<8>>& ZCodeRoutine::print() {
    std::vector<std::bitset<8>> instructions;
    instructions.push_back(std::bitset<8>(localVariables));
    std::vector<std::bitset<8>> superInstructions = ZCodeContainer::print();
    Utils::append(instructions,superInstructions);
    return instructions;
}