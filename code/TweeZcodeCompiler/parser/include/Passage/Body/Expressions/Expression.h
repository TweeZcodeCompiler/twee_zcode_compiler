//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../BodyPart.h"

#include <string>

enum Operator {
    ADD, MUL, SUB, DIV, MOD, LT, LTE, GT, GTE, IS, AND, OR, TO, NEQ, NOT
};
enum FunctionName {
    RANDOM, PREVIOUS, TURNS, VISITED
};


class Expression : public BodyPart {

public:

    virtual std::string to_string() = 0;

    virtual Expression *clone() const = 0;

};

#endif //EXPRESSION_H
