//
// Created by tobias on 6/27/15.
//

#include "include/Passage/Body/Expressions/Variable.h"
#include <string>

Variable::Variable(std::string name) {
    this->name = name;
}

std::string Variable::getName() const {
    return this->name;
}

std::string Variable::to_string() const {
    return "Text: " + this->getName();
}