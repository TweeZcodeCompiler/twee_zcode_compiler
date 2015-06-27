//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Variable.h"

template<class T>
Variable::Variable(T value) : value(value) { }

template<class T>
T Variable::getValue() const {
    return this->value;
}

std::string Variable::to_string() const {
    return "Variable: " + this->getValue();
}