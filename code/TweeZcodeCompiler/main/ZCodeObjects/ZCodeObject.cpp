//
// Created by philip on 27.06.15.
//

#include "ZCodeObject.h"

void ZCodeObject::add(ZCodeObject *Child) {
    children.push_back(Child);
    Child->parrent = this;
    Child->offset = this->offset+ this->size;
    this->size += Child->getSize();
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
    if(parrent != NULL){
        parrent->revalidate();
    }
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