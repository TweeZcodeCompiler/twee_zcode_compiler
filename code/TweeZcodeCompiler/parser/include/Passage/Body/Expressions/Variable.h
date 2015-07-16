//
// Created by tobias on 6/27/15.
//

#ifndef PROJECT_VARIABLE_H
#define PROJECT_VARIABLE_H

#include "Expression.h"
#include <string>

class Variable : public Expression {

private:
    std::string name;

public:

    Variable(std::string name);

    std::string getName() const;

    std::string to_string() const;

    virtual Variable* clone() const {
        return new Variable(*this);
    }
};


#endif //PROJECT_VARIABLE_H
