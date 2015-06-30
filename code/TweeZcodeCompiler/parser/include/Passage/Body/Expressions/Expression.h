//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

enum UnOps {
    MINUS=0, PlUS, LNOT
};

enum BinOps {
    ADD=0, MUL, SUB, DIV, MOD, LAND, LOR, LT, LTE, GT, GTE, IS, NEQ, TO
};

enum FunctionName {
    RANDOM=0, PREVIOUS, TURNS, VISITED
};

class Expression {

public:

    virtual std::string to_string() const = 0;

    virtual Expression *clone() const = 0;

};

#endif //EXPRESSION_H
