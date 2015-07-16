//
// Created by lars on 05.06.15.
//

#ifndef BODYPART_H
#define BODYPART_H

#include "IBodyPartsVisitor.h"

#include <string>

class BodyPart {

public:
    virtual ~BodyPart() { };

    virtual std::string to_string() const = 0;

    virtual BodyPart *clone() const = 0;

    virtual void accept(IBodyPartsVisitor&) const = 0;
};

#endif //BODYPART_H
