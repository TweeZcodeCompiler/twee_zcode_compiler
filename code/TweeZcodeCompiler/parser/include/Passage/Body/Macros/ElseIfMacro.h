
#ifndef ELSEIFMACRO_H
#define ELSEIFMACRO_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../IBodyPartsVisitor.h"

#include <string>
#include <memory>

//called this ElseIfMacro instead of ElseIf because I'm afraid of the c++ lexer :(

class ElseIfMacro : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    ElseIfMacro(const Expression *);

    ElseIfMacro(const Expression &);

    ElseIfMacro(const ElseIfMacro &ifmacro) {
        this->expression.reset(ifmacro.getExpression()->clone());
    }

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual ElseIfMacro *clone() const {
        return new ElseIfMacro(*this);
    }

    void accept(IBodyPartsVisitor&) const;
};

#endif //ELSEIFMACRO_H
