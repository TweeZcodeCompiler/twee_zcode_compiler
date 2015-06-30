//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

enum class UnOps {
    MINUS=0, PLUS, NOT
};

//if you want to change the order of this, I'll cut you ~georg
enum class BinOps {
    ADD=0, MUL, SUB, DIV, MOD, AND, OR, LT, LTE, GT, GTE, IS, NEQ, TO
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
