//
// Created by lars on 05.06.15.
//

#ifndef BODY_H
#define BODY_H

#include "Body/BodyPart.h"

#include <vector>
#include <string>
#include <memory>

class Body {

private:
    std::vector<std::unique_ptr<BodyPart>> bodyparts;

public:

    Body();

    std::vector<std::unique_ptr<BodyPart>> &getBodyParts();

    Body &operator+=(const BodyPart &);

    std::string to_string();

};

#endif //BODY_H
