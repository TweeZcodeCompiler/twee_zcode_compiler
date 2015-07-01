
#ifndef VISITED_H
#define VISITED_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Visited : public Macro {

private:
    std::string passage;

public:

    Visited(std::string);

    Visited(const Visited &display) {
        this->passage = display.getPassage();
    }

    const std::string &getPassage() const;

    std::string to_string() const;

    virtual Visited *clone() const {
        return new Visited(*this);
    }

};

#endif //VISITED_H
