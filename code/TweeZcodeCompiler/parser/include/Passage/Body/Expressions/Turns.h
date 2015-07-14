
#ifndef TURNS_H
#define TURNS_H

#include "Expression.h"

#include <string>
#include <memory>

class Turns : public Expression {

public:

    Turns();

    std::string to_string() const;

    virtual Turns *clone() const {
        return new Turns(*this);
    }
};

#endif //TURNS_H
