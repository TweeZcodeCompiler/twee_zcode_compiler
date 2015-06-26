//
// Created by lars on 25.06.15.
//

#ifndef ELSE_H
#define ELSE_H

#include "../BodyPart.h"

#include <string>

class Else : public Bodypart {

public:

    Else();

    const std::string to_string() const;

    virtual Else *clone() const {
        return new Else(*this);
    }

};

#endif //ELSE_H
