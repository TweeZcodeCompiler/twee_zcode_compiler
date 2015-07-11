#include "include/Passage/Body/Macros/ElseIfMacro.h"

ElseIfMacro::ElseIfMacro(const Expression &expression) {
    this->expression = std::unique_ptr<Expression>(expression.clone());
}

ElseIfMacro::ElseIfMacro(const Expression *expression) {
    this->expression = std::unique_ptr<Expression>(expression->clone());
}

const std::unique_ptr<Expression> &ElseIfMacro::getExpression() const {
    return this->expression;
}

std::string ElseIfMacro::to_string() const {
    return "ElseIfMacro: " + this->getExpression()->to_string();
}

void ElseIfMacro::accept(BodyInspector bodyInspector) {
    bodyInspector.visit(*this);
}