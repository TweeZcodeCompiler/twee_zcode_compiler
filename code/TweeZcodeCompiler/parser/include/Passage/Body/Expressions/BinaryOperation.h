//
// Created by lars on 24.06.15.
//

#ifndef OPERATOR_H
#define OPERATOR_H

#include "Expression.h"

#include <string>
#include <memory>

class BinaryOperation : public Expression {

private:

    BinOps op;

    std::unique_ptr<Expression> leftSide;
    std::unique_ptr<Expression> rightSide;

public:

    BinaryOperation(BinOps, const Expression *, const Expression *);

    BinaryOperation(BinOps, const Expression &, const Expression &);


    BinaryOperation(const BinaryOperation &binaryOperation) {
        this->leftSide.reset(binaryOperation.getLeftSide()->clone());
        this->rightSide.reset(binaryOperation.getRightSide()->clone());
    }

    BinOps getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual BinaryOperation *clone() const {
        return new BinaryOperation(*this);
    }

    std::string to_string() const;

};

#endif //BINARYOPERATOR_H
