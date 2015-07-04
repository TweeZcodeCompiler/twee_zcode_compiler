//
// Created by lars on 05.06.15.
//

#include "include/Passage/Passage.h"
#include "include/Passage/Head.h"
#include "include/Passage/Body.h"

#include <string>

Passage::Passage(Head &head, Body &body) : head(Head(head)), body(Body(body)) {

}

const Head& Passage::getHead() const {
    return this->head;
}

const Body& Passage::getBody() const {
    return this->body;
}

std::string Passage::to_string() const {
    return std::string("Passage \n") + this->getHead().to_string() + std::string("\n") + this->getBody().to_string();
}

