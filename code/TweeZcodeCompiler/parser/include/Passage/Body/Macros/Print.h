//
// Created by lars on 24.06.15.
//

#ifndef PRINT_H
#define PRINT_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Print : public Marco {


public:

    Print(std::unique_ptr<Expression> &);

    const std::unique_ptr<Expression> &getExpression();

    std::string to_string();

    virtual Print *clone() const {
        return new Print(*this);
    }

};

#endif //PRINT_H
