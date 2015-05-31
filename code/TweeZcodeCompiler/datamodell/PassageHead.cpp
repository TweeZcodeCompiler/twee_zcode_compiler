//
// Created by lars on 31.05.15.
//

#include "include/PassageStructure/PassageHead.h"

#include <string>

PassageHead::PassageHead(std::string name) {
    this->name = name;
}

std::string Passage::getName() {

    return this->name;

}

std::string Passage::to_string() {

    return this->name;

}

std::string Passage::to_ZASS() {

}