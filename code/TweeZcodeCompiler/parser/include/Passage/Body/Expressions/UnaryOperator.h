//
// Created by lars on 24.06.15.
//

#ifndef UNARYOPERATOR_H
#define UNARYOPERATOR_H

#include "Expression.h"

#include <string>

class UnaryOperator : public Expression {

private:

    Operator op;
    std::unique_ptr <Expression> expression;

public:

    UnaryOperator(Operator, std::unique_ptr <Expression>);

    Operator getOperator() const;

    const std::unique_ptr <Expression> getExpression();

    std::string to_string();

    virtual UnaryOperator *clone() const {
        return new UnaryOperator(*this);
    }

};

#endif //UNARYOPERATOR_H
