#include "include/Passage/Body/Macros/SetMacro.h"
#include "include/Passage/Body/BodyPartsVisitor.h"

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

void SetMacro::accept(BodyPartsVisitor& visitor) {
    visitor.visit(*this);
}