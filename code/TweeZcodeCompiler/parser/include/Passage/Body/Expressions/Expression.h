//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

enum UnOps {
    MINUS, PlUS, LNOT
};

enum BinOps {
    ADD, MUL, SUB, DIV, MOD, LAND, LOR, LT, LTE, GT, GTE, IS, NEQ, TO
};

enum FunctionName {
    RANDOM, PREVIOUS, TURNS, VISITED
};

class Expression {

public:

    virtual std::string to_string() const = 0;

    virtual Expression *clone() const = 0;

};

#endif //EXPRESSION_H
