
#ifndef PREVIOUS_H
#define PREVIOUS_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Previous : public Macro {

public:

    Previous();

    std::string to_string() const;

    virtual Previous *clone() const {
        return new Previous(*this);
    }

};

#endif //PREVIOUS_H
