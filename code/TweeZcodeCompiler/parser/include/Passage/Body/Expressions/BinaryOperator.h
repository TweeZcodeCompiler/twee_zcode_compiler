//
// Created by lars on 24.06.15.
//

#ifndef BINARYOPERATOR_H
#define BINARYOPERATOR_H

#include "Expression.h"

#include <string>
#include <memory>

class BinaryOperator : public Expression {

private:

    Operator op;
    std::unique_ptr<Expression> leftSide;
    std::unique_ptr<Expression> rightSide;

public:

    BinaryOperator(Operator, std::unique_ptr<Expression>, std::unique_ptr<Expression>);

    Operator getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual BinaryOperator *clone() const {
        return new BinaryOperator(*this);
    }

    std::string to_string();

};

#endif //BINARYOPERATOR_H
