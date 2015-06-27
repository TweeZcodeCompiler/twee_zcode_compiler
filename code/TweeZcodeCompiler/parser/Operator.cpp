//
// Created by lars on 26.06.15.
//

#include "include/Passage/Body/Expressions/Operator.h"

template<class T>
Operator<T>::Operator(T op, std::unique_ptr<Expression> leftSide, std::unique_ptr<Expression> rightSide) {

    this->op = op;
    this->leftSide = leftSide;
    this->rightSide = rightSide;

}

template<class T>
T Operator<T>::getOperator() const {
    return this->op;
}

template<class T>
const std::unique_ptr<Expression> &Operator<T>::getLeftSide() const {
    return this->leftSide;
}

template<class T>
const std::unique_ptr<Expression> &Operator<T>::getRightSide() const {
    return this->rightSide;
}

template<class T>
std::string Operator<T>::to_string() const {
    return "Operator: " + this->getOperator() + "\n" + "Left Side: " + this->getLeftSide()->to_string() + "\n"
            "Right Side: " + this->getRightSide()->to_string() + "\n";
}