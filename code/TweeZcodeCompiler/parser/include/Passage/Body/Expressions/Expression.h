//
// Created by lars on 24.06.15.
//

#ifndef EXPRESSION_H
#define EXPRESSION_H

#include <string>

enum class UnOps {
    MINUS=0, PLUS=1, NOT=2
};

//if you want to change the order of this, I'll cut you ~georg
enum class BinOps {
    ADD=0,
    MUL=1,
    SUB=2,
    DIV=3,
    MOD=4,
    AND=5,
    OR=6,
    LT=7,
    LTE=8,
    GT=9,
    GTE=10,
    IS=11,
    NEQ=12,
    TO=13
};

enum FunctionName {
    RANDOM=0, PREVIOUS, TURNS, VISITED
};

class Expression {

public:

    virtual ~Expression() {};

    virtual std::string to_string() const = 0;

    virtual Expression *clone() const = 0;

};

#endif //EXPRESSION_H
