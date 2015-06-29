//
// Created by lars on 24.06.15.
//

#ifndef OPERATOR_H
#define OPERATOR_H

#include "Expression.h"

#include <string>
#include <memory>

template<class T>
class Operator : public Expression {

private:

    T op;

    std::unique_ptr<Expression> leftSide;
    std::unique_ptr<Expression> rightSide;

public:

    Operator(T, std::unique_ptr<Expression>, std::unique_ptr<Expression>);

    T getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual Operator *clone() const {
        return new Operator(*this);
    }

    std::string to_string() const;

};

#endif //BINARYOPERATOR_H
