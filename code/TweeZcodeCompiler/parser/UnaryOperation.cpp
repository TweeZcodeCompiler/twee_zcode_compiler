//
// Created by iguana on 30.06.15.
//

#include "include/Passage/Body/Expressions/UnaryOperation.h"

#include <memory>

UnaryOperation::UnaryOperation(UnOps op, const Expression *expression) {
    this->op = op;
    this->expression = std::unique_ptr<Expression>(expression->clone());
}

UnaryOperation::UnaryOperation(UnOps op, const Expression &expression) {
    this->op = op;
    this->expression = std::unique_ptr<Expression>(expression.clone());
}

UnOps UnaryOperation::getOperator() const {
    return this->op;
}

std::string UnaryOperation::getOperatorString() const {
    std::string result;
    switch (this->getOperator()) {

        case UnOps::MINUS:
            result = "MINUS";
            break;
        case UnOps::PLUS:
            result = "PLUS";
            break;
        case UnOps::NOT:
            result = "NOT";
            break;
        default:
            result = "Unknown Operator";
            break;
    }
    return result;
}

const std::unique_ptr<Expression> &UnaryOperation::getExpression() const {
    return this->expression;
}


std::string UnaryOperation::to_string() const {
    return "UnaryOperation: " + this->getOperatorString() + "( " + this->getExpression()->to_string()+" )";
}
