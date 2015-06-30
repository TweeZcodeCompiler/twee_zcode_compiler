//
// Created by philip on 27.06.15.
//

#include "ZCodeRoutine.h"
#include "../Utils.h"

std::map<std::string,std::shared_ptr<ZCodeRoutine>> ZCodeRoutine::routines = std::map<std::string,std::shared_ptr<ZCodeRoutine>>();

std::shared_ptr<ZCodeRoutine> ZCodeRoutine::getOrCreateRoutine(std::string name, std::uint8_t locVariables) {
    if (routines.count(name) == 0) {
        auto routine = std::shared_ptr<ZCodeRoutine>(new ZCodeRoutine(locVariables));
        routine ->displayName = name;
        routines.insert(std::pair<std::string, std::shared_ptr<ZCodeRoutine>>(name,routine));
        return routine;
    } else {
        auto routine = routines.at(name);
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