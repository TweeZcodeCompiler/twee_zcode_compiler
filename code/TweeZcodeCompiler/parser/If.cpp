//
// Created by lars on 25.06.15.
//

#include "include/Passage/Body/Macros/Macro.h"
#include "include/Passage/Body/Macros/If.h"
#include "include/Passage/Body/Expressions/Expression.h"

#include <string>

If::If(std::unique_ptr<::Expression> expression) : expression(expression) { }

const std::unique_ptr<Expression> &If::getExpression() const {
    return this->expression;
}

std::string If::to_string() const {
    return "If \n  Expression: " + this->getExpression()->to_string();
}