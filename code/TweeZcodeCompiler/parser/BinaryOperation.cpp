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


const std::unique_ptr<Expression> &BinaryOperation::getLeftSide() const {
    return this->leftSide;
}


const std::unique_ptr<Expression> &BinaryOperation::getRightSide() const {
    return this->rightSide;
}


std::string BinaryOperation::to_string() const {
    return "Binops"; //"BinaryOperation: " + this->getOperator() + "\n" + "Left Side: " + this->getLeftSide()->to_string() + "\n"
    //"Right Side: " + this->getRightSide()->to_string() + "\n";
}