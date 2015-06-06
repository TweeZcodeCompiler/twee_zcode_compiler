//
// Created by micha on 14.05.15.
//

#include "include/Passage.h"


Passage::Passage(std::string name, Body& body) : body(body) {
    passageName = name;
    id = 0;
}

Body Passage::getBody(){
    return this->body;
}

std::string Passage::getPassageName(){
    return this->passageName;
}
