//
// Created by lars on 25.06.15.
//

#ifndef ENDIF_H
#define ENDIF_H

#include "../BodyPart.h"

#include <string>

class EndIf : public Bodypart {

public:

    EndIf();

    std::string to_string() const;

    virtual EndIf *clone() const {
        return new EndIf(*this);
    }

};

#endif //ENDIF_H
