//
// Created by lars on 24.06.15.
//

#ifndef VARIABLE_H
#define VARIABLE_H

#include "Expression.h"

#include <string>

template<class T>
class Variable : public Expression {

private:
    T value;

public:

    Variable(T);

    T getValue() const;

    std::string to_string() const;

    virtual Variable *clone() const {
        return new Variable(*this);
    }

};

#endif //VARIABLE_H
