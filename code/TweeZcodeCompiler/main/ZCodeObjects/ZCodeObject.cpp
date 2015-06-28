//
// Created by philip on 27.06.15.
//

#include <ostream>
#include <iostream>
#include <plog/Log.h>
#include "ZCodeObject.h"

void ZCodeObject::add(ZCodeObject *Child) {
    std::cout << "add '"<<Child->displayName<<"' to '"<< displayName <<"'"<< std::endl;
    children.push_back(Child);
    Child->parrent = this;
    Child->offset = this->offset+ this->size;
   setSize(this->size+ Child->getSize());
    ZCodeObject *parrent = this->parrent;
    if(parrent != NULL){
        parrent->revalidate();
    }
}

void ZCodeObject::setSize(size_t size) {
    if(size == this->size){
        return;
    }
    this->size = size;
    std::cout << "size of '"<< displayName<<"' is set to "<< this->size << std::endl;
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
    std::cout << offset << "\t\t|" << displayName<<std::endl;
    for(size_t i = 0; i < children.size(); i++){
        ZCodeObject *child = children.at(i);
        child->printMemory();
    }
}