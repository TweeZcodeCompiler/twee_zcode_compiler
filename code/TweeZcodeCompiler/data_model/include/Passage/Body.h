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
    vector <BodyPart> bodyparts;

public:

    Body();

    vector <BodyPart> getBodyParts();

    Body &operator+=(const BodyPart &);

    std::string to_string();

};

#endif //BODY_H
