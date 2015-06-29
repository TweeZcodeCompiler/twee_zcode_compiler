//
// Created by iguana on 30.06.15.
//

#include "include/Passage/Body/Expressions/UnaryOperation.h"

#include <memory>

UnaryOperation::UnaryOperation(UnOps op, const Expression *unaryOperation) {
    this->op = op;
    this->expression = std::unique_ptr<Expression>(unaryOperation->clone());
}

UnaryOperation::UnaryOperation(UnOps op, const Expression &unaryOperation) {
    this->op = op;
    this->expression = std::unique_ptr<Expression>(unaryOperation.clone());
}

UnOps UnaryOperation::getOperator() const {
    return this->op;
}

const std::unique_ptr<Expression> &UnaryOperation::getExpression() const {
    return this->expression;
}


std::string UnaryOperation::to_string() const {
    return "UnOps";
}
