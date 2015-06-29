//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include "../BodyPart.h"

#include <string>

enum ArithmeticOperation {
    ADD, MUL, SUB, DIV, MOD, MINUS
};

enum LogicalOperation {
    AND, OR, NOT
};

enum RelationOperation {
    LT, LTE, GT, GTE, IS, NEQ
};

enum AssignmentOperation {
    TO
};

enum FunctionName {
    RANDOM, PREVIOUS, TURNS, VISITED
};

class Expression : public BodyPart {

public:

    virtual std::string to_string() const = 0;

    virtual Expression *clone() const = 0;

};

#endif //EXPRESSION_H
