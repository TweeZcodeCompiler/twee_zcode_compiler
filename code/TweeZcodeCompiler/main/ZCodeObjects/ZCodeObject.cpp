//
// Created by philip on 27.06.15.
//

#include <ostream>
#include <iostream>
#include <plog/Log.h>
#include "ZCodeObject.h"

void ZCodeObject::add(std::shared_ptr<ZCodeObject> Child) {
    LOG_DEBUG << "add '"<<Child->displayName<<"' to '"<< displayName <<"'";
    children.push_back(Child);
    Child->parent = share();
    Child->offset = this->offset+ this->size;
   setSize(this->size+ Child->getSize());
    if(parent != NULL){
        parent->revalidate();
    }
}

void ZCodeObject::setSize(size_t size) {
    if(size == this->size){
        return;
    }
    this->size = size;
    LOG_DEBUG << "size of '"<< displayName<<"' is set to "<< this->size;
}

size_t ZCodeObject::getSize() {
    return this->size;
}

size_t ZCodeObject::getOffset() {
    return this->offset;
}

void ZCodeObject::setOffset(size_t offset) {
    this->offset = offset;
}

void ZCodeObject::printMemory() {
    LOG_DEBUG << offset << "\t\t|" << displayName;
    for(size_t i = 0; i < children.size(); i++){
        std::shared_ptr<ZCodeObject> child = children.at(i);
        child->printMemory();
    }
}