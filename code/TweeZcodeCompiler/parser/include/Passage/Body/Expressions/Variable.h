//
// Created by lars on 24.06.15.
//

#ifndef VARIABLE_H
#define VARIABLE_H

#include "Expression.h"

#include <string>

class Variable : public Expression {

private:


public:

    Variable(Operator, Expression &);


    std::string to_string();

    virtual Variable *clone() const {
        return new Variable(*this);
    }

};

#endif //VARIABLE_H
