//
// Created by lars on 24.06.15.
//

#ifndef IF_H
#define IF_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class If : public Marco {

public:

    If(std::unique_ptr<Expression>);

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual If *clone() const {
        return new If(*this);
    }

};

#endif //IFELSE_H
