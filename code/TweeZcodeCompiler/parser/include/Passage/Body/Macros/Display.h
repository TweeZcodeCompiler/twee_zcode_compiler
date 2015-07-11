//
// Created by lars on 24.06.15.
//

#ifndef DISPLAY_H
#define DISPLAY_H

#include "Macro.h"
#include "../Expressions/Expression.h"

#include <string>
#include <memory>

class Display : public Macro {

private:
    std::string passage;

public:

    Display(std::string);

    Display(const Display &display) {
        this->passage = display.getPassage();
    }

    const std::string &getPassage() const;

    std::string to_string() const;

    virtual Display *clone() const {
        return new Display(*this);
    }

    void accept(BodyInspector) const;

};

#endif //DISPLAY_H
