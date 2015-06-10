//
// Created by lars on 05.06.15.
//

#ifndef BODY_H
#define BODY_H

#include "Body/BodyPart.h"

#include <vector>
#include <string>

class Body {

private:
    std::vector<BodyPart*> bodyparts;

public:

    Body();
    ~Body();

    std::vector<BodyPart*> &getBodyParts();

    Body &operator+=(BodyPart*);

    std::string to_string();

};

#endif //BODY_H
