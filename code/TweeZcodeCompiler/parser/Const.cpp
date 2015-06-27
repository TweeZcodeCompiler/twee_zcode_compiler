//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Const.h"

template<class T>
Const<T>::Const(const T value) : value(value) { }

template<class T>
const T Const<T>::getValue() const {
    return this->value;
}

template<class T>
std::string Const<T>::to_string() const {
    return "Const: " + this->getValue();
}