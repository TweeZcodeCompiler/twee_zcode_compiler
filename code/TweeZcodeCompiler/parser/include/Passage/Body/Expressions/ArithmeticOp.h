//
// Created by lars on 26.06.15.
//

#ifndef ARITHMETICOP_H
#define ARITHMETICOP_H

#include "Operator.h"

#include <string>
#include <memory>


class ArithmeticOp : public Operator {

public:

    ArithmeticOp(ArithmeticOperation, std::unique_ptr<Expression> &, std::unique_ptr<Expression> &);

    ArithmeticOperation getOperator() const;

    const std::unique_ptr<Expression> &getLeftSide() const;

    const std::unique_ptr<Expression> &getRightSide() const;

    virtual ArithmeticOp *clone() const {
        return new ArithmeticOp(*this);
    }

    std::string to_string() const;

};

#endif //ARITHMETICOP_H
