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

    T getValue() const {
        return this->value;
    }

    std::string to_string() const {
        return "Const: " + std::to_string(this->getValue());
    }

    virtual Const* clone() const {
        return new Const(*this);
    }
};

template<>
class Const<std::string> : public Expression {

private:
    const std::string value;

public:

    Const<std::string>(const std::string value) : value(value) { }

    std::string getValue() const {
        return this->value;
    }

    std::string to_string() const {
        return "Const: " + this->getValue();
    }

    virtual Const* clone() const {
        return new Const(*this);
    }
};


#endif //CONST_H
