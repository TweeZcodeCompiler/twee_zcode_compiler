//
// Created by lars on 24.06.15.
//

#ifndef OPERATOR_H
#define OPERATOR_H

#include "Expression.h"

#include <string>
#include <memory>

class Operator : public Expression {

private:

    int op;
    std::unique_ptr<Expression> leftSide;
    std::unique_ptr<Expression> rightSide;

public:

    int getOperator() = 0;

    virtual std::unique_ptr<Expression> &getLeftSide() = 0;

    virtual std::unique_ptr<Expression> &getRightSide() = 0;

    virtual Operator *clone() = 0;

    virtual std::string to_string() = 0;

};

#endif //BINARYOPERATOR_H
