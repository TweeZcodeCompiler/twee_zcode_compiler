//
// Created by philip on 27.06.15.
//

#include <iostream>
#include "ZCodeContainer.h"
#include "../Utils.h"

bool ZCodeContainer::revalidate() {
    size_t calculatedSize = 0;
    bool validatedAll = true;
    while(validatedAll){
        validatedAll = false;
        calculatedSize = 0;
        for(size_t i = 0; i < children.size(); i++){
            ZCodeObject *child = children.at(i);
            child->size = this->offset+ calculatedSize;
            if(child->revalidate()){
                validatedAll = true;
            }
            calculatedSize += child->getSize();
        }
    }
    size_t currentSize = this->size;
    if(this->size != calculatedSize){
        this->size = calculatedSize;
        size_t after = this->size;
        return true;
    }
    return false;
}

void ZCodeContainer::print(std::vector<std::bitset<8>> &code) {
    std::vector<std::bitset<8>> instructions = std::vector<std::bitset<8>>();
    for(size_t i = 0; i < children.size(); i++){
        ZCodeObject *child = children.at(i);
        child->print(code);
    }
}