//
// Created by lars on 24.06.15.
//

#ifndef PROJECT_CONSTSTR_H
#define PROJECT_CONSTSTR_H

#include "Expression.h"

#include <string>

class ConstStr : public Expression {

private:

    std::string value;

public:

    ConstStr(std::string);

    std::string getString() const;

    std::string to_string();

    virtual ConstStr *clone() const {
        return new ConstStr(*this);
    }

};

#endif //PROJECT_CONSTSTR_H
