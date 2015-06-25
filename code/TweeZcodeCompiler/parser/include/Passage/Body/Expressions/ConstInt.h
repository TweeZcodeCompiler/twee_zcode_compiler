//
// Created by lars on 24.06.15.
//

#ifndef PROJECT_CONSTINT_H
#define PROJECT_CONSTINT_H

#include "Expression.h"

#include <string>

class ConstInt : public Expression {

private:

    int value;

public:

    ConstInt(int);

    int getValue() const;

    std::string to_string();

    virtual ConstInt *clone() const {
        return new ConstInt(*this);
    }

};

#endif //PROJECT_CONSTINT_H
