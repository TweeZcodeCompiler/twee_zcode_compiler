//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Macros/Display.h"

Display::Display(const Expression &expression) {
    this->expression = std::unique_ptr(expression.clone());
}

Display::Display(const Expression *expression) {
    this->expression = std::unique_ptr(expression->clone());
}

const std::unique_ptr<Expression> &Display::getExpression() const {
    return this->expression;
}

std::string Display::to_string() const {
    return "Display: " + this->getExpression()->to_string();
}