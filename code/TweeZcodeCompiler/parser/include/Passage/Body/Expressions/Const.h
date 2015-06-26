//
// Created by lars on 24.06.15.
//

#ifndef PROJECT_CONST_H
#define PROJECT_CONST_H

#include "Expression.h"

#include <string>

template<class T>
class Const : public Expression {

private:
    const T value;

public:

    Const(const T);

    const T getValue() const;

    std::string to_string() const;

    virtual Const *clone() const {
        return new Const(*this);
    }

};

#endif //PROJECT_CONSTINT_H
