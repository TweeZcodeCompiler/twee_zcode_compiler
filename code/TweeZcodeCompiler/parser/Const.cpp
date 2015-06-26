//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Const.h"

template<class T>
Const::Const(const T value) : value(value) { }

template<class T>
const T Const::getValue() const {
    return this->value;
}

std::string Const::to_string() const {
    return "Const: " + this->getValue();
}