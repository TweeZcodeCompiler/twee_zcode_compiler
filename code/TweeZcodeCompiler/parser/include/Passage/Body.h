//
// Created by lars on 05.06.15.
//

#ifndef BODY_H
#define BODY_H

#include "Body/BodyPart.h"

#include <vector>
#include <memory>
#include <string>

class Body {
private:
    std::vector<std::unique_ptr<BodyPart>> bodyparts;
    Body(const Body& that) = delete;

public:

    std::vector<std::unique_ptr<BodyPart>> &getBodyParts();

    Body& operator+=(std::unique_ptr<BodyPart>&);
    Body& operator+=(BodyPart* bodyPart);

    std::string to_string();
};

#endif //BODY_H
