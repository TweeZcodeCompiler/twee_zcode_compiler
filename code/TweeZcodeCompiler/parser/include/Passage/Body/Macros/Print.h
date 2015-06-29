//
// Created by lars on 24.06.15.
//

#ifndef PRINT_H
#define PRINT_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Print : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    Print(const Expression *);

    Print(const Expression &);

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual Print *clone() const {
        return new Print(*this);
    }

};


#endif //PRINT_H
