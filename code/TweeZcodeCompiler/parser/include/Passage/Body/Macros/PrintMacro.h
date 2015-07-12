//
// Created by lars on 24.06.15.
//

#ifndef PRINTMACRO_H
#define PRINTMACRO_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../IBodyPartsVisitor.h"

#include <string>
#include <memory>

class PrintMacro : public Macro {

private:
    std::unique_ptr<Expression> expression;

public:

    PrintMacro(const Expression *);

    PrintMacro(const Expression &);

    PrintMacro(const PrintMacro &printMacro) {
        this->expression.reset(printMacro.getExpression()->clone());
    }

    const std::unique_ptr<Expression> &getExpression() const;

    std::string to_string() const;

    virtual PrintMacro *clone() const {
        return new PrintMacro(*this);
    }

    void accept(const IBodyPartsVisitor&);

};


#endif //PRINTMACRO_H
