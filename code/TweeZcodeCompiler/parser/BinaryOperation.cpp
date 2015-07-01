//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/BinaryOperation.h"

BinaryOperation::BinaryOperation(BinOps op, const Expression *leftSide, const Expression *rightSide) {
    this->op = op;
    this->leftSide = std::unique_ptr<Expression>(leftSide->clone());
    this->rightSide = std::unique_ptr<Expression>(rightSide->clone());
}

BinaryOperation::BinaryOperation(BinOps op, const Expression &leftSide, const Expression &rightSide) {
    this->op = op;
    this->leftSide = std::unique_ptr<Expression>(leftSide.clone());
    this->rightSide = std::unique_ptr<Expression>(rightSide.clone());
}


BinOps BinaryOperation::getOperator() const {
    return this->op;
}

std::string BinaryOperation::getOperatorString() const {
    std::string result;
    switch (this->getOperator()) {
        case BinOps::ADD:
            result = "ADD";
            break;
        case BinOps::MUL:
            result = "MUL";
            break;
        case BinOps::SUB:
            result = "SUB";
            break;
        case BinOps::DIV:
            result = "DIV";
            break;
        case BinOps::MOD:
            result = "MOD";
            break;
        case BinOps::AND:
            result = "AND";
            break;
        case BinOps::OR:
            result = "OR";
            break;
        case BinOps::LT:
            result = "LT";
            break;
        case BinOps::LTE:
            result = "LTE";
            break;
        case BinOps::GT:
            result = "GT";
            break;
        case BinOps::GTE:
            result = "GTE";
            break;
        case BinOps::IS:
            result = "IS";
            break;
        case BinOps::NEQ:
            result = "NEQ";
            break;
        case BinOps::TO:
            result = "TO";
            break;
        default:
            result = "Unknown Operator";
            break;
    }
    return result;
}


const std::unique_ptr<Expression> &BinaryOperation::getLeftSide() const {
    return this->leftSide;
}


const std::unique_ptr<Expression> &BinaryOperation::getRightSide() const {
    return this->rightSide;
}


std::string BinaryOperation::to_string() const {
    return "BinaryOperation: (" + this->getLeftSide()->to_string() +
           ") "+ this->getOperatorString() +" (" + this->getRightSide()->to_string()+")";
}