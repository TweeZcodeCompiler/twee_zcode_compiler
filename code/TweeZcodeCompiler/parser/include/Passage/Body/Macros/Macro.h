//
// Created by lars on 24.06.15.
//

#ifndef MACRO_H
#define MACRO_H

#include "../BodyPart.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Marco : public BodyPart {

protected:
    std::unique_ptr<Expression> expression;

public:

    virtual const std::unique_ptr<Expression> &getExpression() const = 0;

    virtual Macro *clone() const = 0;

    std::string to_string() const;

};


#endif //MACRO_H
