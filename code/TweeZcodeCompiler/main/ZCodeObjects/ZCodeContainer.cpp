//
// Created by philip on 27.06.15.
//

#include <iostream>
#include <plog/Log.h>
#include "ZCodeContainer.h"
#include "../Utils.h"
using std::shared_ptr;

bool ZCodeContainer::revalidate() {
    size_t calculatedSize = containerOffset;
    bool validatedAll = true;
    while(validatedAll){
        validatedAll = false;
        calculatedSize = containerOffset;
        for(size_t i = 0; i < children.size(); i++){
            std::shared_ptr<ZCodeObject> child = children.at(i);
            size_t offset = this->offset;
            child->setOffset(offset+ calculatedSize);
            LOG_DEBUG <<" revalidate '"<< child->displayName<<"' in '"<< (displayName) << "'";
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
        std::shared_ptr<ZCodeObject> child = children.at(i);
        child->checkOffset(code);
        child->print(code);
    }
}

std::shared_ptr<ZCodeLabel> ZCodeContainer::getOrCreateLabel(std::string name) {
    if (labels.count(name) == 0) {
        shared_ptr<ZCodeLabel> label = std::shared_ptr<ZCodeLabel>(new ZCodeLabel());
        label ->displayName = "["+name+"]";
        labels.insert(std::pair<std::string, shared_ptr<ZCodeLabel>>(name,label));
        return label;
    } else {
        shared_ptr<ZCodeLabel> label = labels.at(name);
        return label;
    }
}