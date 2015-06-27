//
// Created by philip on 27.06.15.
//

#include "ZCodeContainer.h"
#include "../Utils.h"

bool ZCodeContainer::revalidate() {
    size_t size = 0;
    bool validatedAll = true;
    while(validatedAll){
        validatedAll = false;
        size = 0;
        for(size_t i = 0; i < children.size(); i++){
            ZCodeObject child = children.at(i);
            child.size = this->offset+size;
            if(child.revalidate()){
                validatedAll = true;
            }
            size += child.getSize();
        }
    }
    if(this->size != size){
        this->setSize(size);
    }
}

std::vector<std::bitset<8>> &ZCodeContainer::print() {
    std::vector<std::bitset<8>> instructions = std::vector<std::bitset<8>>();
    for(size_t i = 0; i < children.size(); i++){
        ZCodeObject child = children.at(i);
        std::vector<std::bitset<8>> childinstructions = child.print();
        Utils::append(instructions,childinstructions);
    }
    return instructions;
}