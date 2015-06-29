//
// Created by lars on 24.06.15.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Display : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    Display(const Expression *);

    Display(const Expression &);

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual Display *clone() const {
        return new Display(*this);
    }

};

#endif //DISPLAY_H
