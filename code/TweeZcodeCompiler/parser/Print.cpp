//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Macros/Print.h"

Print::Print(std::unique_ptr<Expression> expression) {
    this->expression = expression;
}

const std::unique_ptr<Expression> &Print::getExpression() const {
    return this->expression;
}

std::string Print::to_string() const {
    return "Print: " + this->getExpression()->to_string();
}