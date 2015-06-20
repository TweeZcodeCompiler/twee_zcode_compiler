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


public:

    Body() {}

    ~Body() {}

    Body(const Body& that) {};

    Body& operator=(const Body& that) {return *this;}


    const std::vector<std::unique_ptr<BodyPart>> &getBodyParts() const;

    Body& operator+=(std::unique_ptr<BodyPart>&&);
    Body& operator+=(BodyPart* bodyPart);

    std::string to_string();
};

#endif //BODY_H
