//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Macros/Display.h"

Display::Display(std::unique_ptr<Expression> expression) {
    this->expression = expression;
}

const std::unique_ptr<Expression> &Display::getExpression() const {
    return this->expression;
}

std::string Display::to_string() const {
    return "Display: " + this->getExpression()->to_string();
}