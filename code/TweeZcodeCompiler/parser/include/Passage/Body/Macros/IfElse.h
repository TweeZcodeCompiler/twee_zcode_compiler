//
// Created by lars on 24.06.15.
//

#ifndef IFELSE_H
#define IFELSE_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class IfElse : public Marco {

private:
    std::unique_ptr<Expression> elseExpression;


public:

    IfElse(std::unique_ptr<Expression> &, std::unique_ptr<Expression> &);

    const std::unique_ptr<Expression> &getIfExpression();

    const std::unique_ptr<Expression> &getElseExpression();

    std::string to_string();

    virtual IfElse *clone() const {
        return new IfElse(*this);
    }

};

#endif //IFELSE_H
