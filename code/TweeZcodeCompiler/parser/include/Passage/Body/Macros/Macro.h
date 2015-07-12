//
// Created by lars on 24.06.15.
//

#ifndef MACRO_H
#define MACRO_H

#include "../BodyPart.h"
#include "../Expressions/Expression.h"
#include "../BodyPartsVisitor.h"

#include <string>
#include <memory>

class Macro : public BodyPart {

public:

    virtual std::string to_string() const = 0;

    virtual Macro *clone() const = 0;

    virtual void accept(BodyPartsVisitor) = 0;

};


#endif //MACRO_H
