//
// Created by lars on 24.06.15.
//

#ifndef DISPLAYMACRO_H
#define DISPLAYMACRO_H

#include "Macro.h"
#include "../Expressions/Expression.h"
#include "../BodyPartsVisitor.h"

#include <string>
#include <memory>

class DisplayMacro : public Macro {

private:
    std::string passage;

public:

    DisplayMacro(std::string);

    DisplayMacro(const DisplayMacro &displayMacro) {
        this->passage = displayMacro.getPassage();
    }

    const std::string &getPassage() const;

    std::string to_string() const;

    virtual DisplayMacro *clone() const {
        return new DisplayMacro(*this);
    }

    void accept(BodyPartsVisitor&);

};

#endif //DISPLAYMACRO_H
