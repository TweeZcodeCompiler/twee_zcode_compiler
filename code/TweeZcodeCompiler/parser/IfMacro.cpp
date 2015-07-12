#include "include/Passage/Body/Macros/IfMacro.h"
#include "include/Passage/Body/IBodyPartsVisitor.h"

IfMacro::IfMacro(const Expression &expression) {
    this->expression = std::unique_ptr<Expression>(expression.clone());
}

IfMacro::IfMacro(const Expression *expression) {
    this->expression = std::unique_ptr<Expression>(expression->clone());
}

const std::unique_ptr<Expression> &IfMacro::getExpression() const {
    return this->expression;
}

std::string IfMacro::to_string() const {
    return "IfMacro: " + this->getExpression()->to_string();
}

void IfMacro::accept(const IBodyPartsVisitor & visitor) {
    visitor.visit(*this);
}