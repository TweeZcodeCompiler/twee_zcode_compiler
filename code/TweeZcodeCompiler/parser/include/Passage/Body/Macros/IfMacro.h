
#ifndef IFMACRO_H
#define IFMACRO_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../BodyPartsVisitor.h"

#include <string>
#include <memory>

//called this IfMacro instead of If because I'm afraid of the c++ lexer :(

class IfMacro : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    IfMacro(const Expression *);

    IfMacro(const Expression &);

    IfMacro(const IfMacro &ifmacro) {
        this->expression.reset(ifmacro.getExpression()->clone());
    }

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual IfMacro *clone() const {
        return new IfMacro(*this);
    }

    void accept(BodyPartsVisitor&);

};

#endif //IFMACRO_H
