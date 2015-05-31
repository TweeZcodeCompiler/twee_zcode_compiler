//
// Created by lars on 31.05.15.
//

#include "include/PassageStructure/Passage.h"
#include "include/PassageStructure/PassageHead.h"

#include <string>

Passage::Passage(Head & head, Body & body) {

    this->head = head;
    this->body = body;

}

std::string Passage::to_string() {

    return this->head.to_string() + this->body.to_string();
}

std::string Passage::to_ZASS() {

    return this->head.to_ZASS() + this->body.to_ZASS();
}