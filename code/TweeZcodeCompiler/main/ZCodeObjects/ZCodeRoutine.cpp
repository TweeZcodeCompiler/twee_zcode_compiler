//
// Created by philip on 27.06.15.
//

#include "ZCodeRoutine.h"
#include "../Utils.h"

std::map<std::string,ZCodeRoutine*> ZCodeRoutine::routines = std::map<std::string,ZCodeRoutine*>();

ZCodeRoutine *ZCodeRoutine::getOrCreateRoutine(std::string name, std::uint8_t locVariables) {
    if (routines.count(name) == 0) {
        ZCodeRoutine *routine = new ZCodeRoutine(0);
        routines.insert(std::pair<std::string, ZCodeRoutine*>(name,routine));
        return routine;
    } else {
        ZCodeRoutine *routine = routines.at(name);
        if(routine->localVariables < locVariables){
            routine->localVariables = locVariables;
        }
        return routine;
    }
}

void ZCodeRoutine::print(std::vector<std::bitset<8>> &code) {
   code.push_back(std::bitset<8>(localVariables));
    ZCodeContainer::print(code);
}