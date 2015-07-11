
#ifndef SETMACRO_H
#define SETMACRO_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../../../AssemblyGeneratorVisitor.h"

#include <string>
#include <memory>

class SetMacro : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    SetMacro(const Expression *);

    SetMacro(const Expression &);

    SetMacro(const SetMacro &ifmacro) {
        this->expression.reset(ifmacro.getExpression()->clone());
    }

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual SetMacro *clone() const {
        return new SetMacro(*this);
    }

    void accept(AssemblyGeneratorVisitor) const;

};

#endif //SETMACRO_H
