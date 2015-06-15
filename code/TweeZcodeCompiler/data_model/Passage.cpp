//
// Created by lars on 05.06.15.
//

#include "include/Passage/Passage.h"
#include "include/Passage/Head.h"
#include "include/Passage/Body.h"

#include <string>

Passage::Passage(Head &head, Body &body) {

    this->head = head;
    this->body = body;

}

Head Passage::getHead() {

    return this->head;

}

Body Passage::getBody() {

    return this->body;

}

std::string Passage::to_string() {

    return this->head.to_string() + std::endl + this->body.to_string();

}

