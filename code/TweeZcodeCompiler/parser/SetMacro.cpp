#include "include/Passage/Body/Macros/SetMacro.h"
#include "include/Passage/Body/IBodyPartsVisitor.h"

SetMacro::SetMacro(const Expression &expression) {
    this->expression = std::unique_ptr<Expression>(expression.clone());
}

SetMacro::SetMacro(const Expression *expression) {
    this->expression = std::unique_ptr<Expression>(expression->clone());
}

const std::unique_ptr<Expression> &SetMacro::getExpression() const {
    return this->expression;
}

std::string SetMacro::to_string() const {
    return "SetMacro: " + this->getExpression()->to_string();
}

void SetMacro::accept(IBodyPartsVisitor & visitor) const {
    visitor.visit(*this);
}