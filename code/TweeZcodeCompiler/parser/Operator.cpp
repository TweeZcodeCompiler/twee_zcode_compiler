//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Operator.h"

template<class T>
Operator::Operator(T op, std::unique_ptr<Expression> leftSide, std::unique_ptr<Expression> rightSide) {

    this->op = op;
    this->leftSide = leftSide;
    this->rightSide = rightSide;

}

template<class T>
T Operator::getOperator() const {
    return this->op;
}

const std::unique_ptr<Expression> &Operator::getLeftSide() const {
    return this->leftSide;
}

const std::unique_ptr<Expression> &Operator::getRightSide() const {
    return this->rightSide;
}

std::string Operator::to_string() const {
    return "Operator: " + this->getOperator() + "\n" + "Left Side: " + this->getLeftSide()->to_string() + "\n"
            "Right Side: " + this->getRightSide()->to_string() + "\n";
}