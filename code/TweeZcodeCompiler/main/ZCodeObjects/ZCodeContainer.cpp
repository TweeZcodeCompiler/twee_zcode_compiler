//
// Created by philip on 27.06.15.
//

#include <iostream>
#include <plog/Log.h>
#include "ZCodeContainer.h"
#include "../Utils.h"

bool ZCodeContainer::revalidate() {
    size_t calculatedSize = containerOffset;
    bool validatedAll = true;
    while(validatedAll){
        validatedAll = false;
        calculatedSize = containerOffset;
        for(size_t i = 0; i < children.size(); i++){
            ZCodeObject *child = children.at(i);
            size_t offset = this->offset;
            child->setOffset(offset+ calculatedSize);
            std::cout <<" revalidate '"<< child->displayName<<"' in '"<< (displayName) << "'"<< std::endl;
            if(child->revalidate()){
                validatedAll = true;
            }
            calculatedSize = calculatedSize + child->size;
        }
    }
    size_t currentSize = this->size;
    if(this->size != calculatedSize){
        setSize(calculatedSize);
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