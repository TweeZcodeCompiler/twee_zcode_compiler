//
// Created by lars on 24.06.15.
//

#ifndef CONST_H
#define CONST_H

#include "Expression.h"

#include <string>

template<class T>
class Const : public Expression {

private:
    const T value;

public:

    Const<T>(const T value) : value(value) { }

    const T getValue() const {
        return this->value;
    }

    std::string to_string() const {
        return "Const: ";
    }

    virtual Const *clone() const {
        return new Const(*this);
    }
};

#endif //CONST_H
