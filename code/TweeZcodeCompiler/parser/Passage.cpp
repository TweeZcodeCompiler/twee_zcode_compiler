//
// Created by lars on 05.06.15.
//

#include "include/Passage/Passage.h"
#include "include/Passage/Head.h"
#include "include/Passage/Body.h"

#include <string>

Passage::Passage(Head &head, Body &body) : head(Head(head)), body(Body(body)) {

}

Head& Passage::getHead() {
    return this->head;
}

Body& Passage::getBody() {
    return this->body;
}

std::string Passage::to_string() {
    return std::string("Passage \n") + this->getHead().to_string() + std::string("\n") + this->getBody().to_string();
}

