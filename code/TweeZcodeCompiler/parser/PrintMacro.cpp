//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Macros/PrintMacro.h"

PrintMacro::PrintMacro(const Expression *expression) {
    this->expression = std::unique_ptr<Expression>(expression->clone());
}

PrintMacro::PrintMacro(const Expression &expression) {
    this->expression = std::unique_ptr<Expression>(expression.clone());
}

const std::unique_ptr<Expression> &PrintMacro::getExpression() const {
    return this->expression;
}

std::string PrintMacro::to_string() const {
    return "PrintMacro: " + this->expression->to_string();
}

void PrintMacro::accept(IBodyPartsVisitor& visitor) const {
    visitor.visit(*this);
}