//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "include/Passage/Body/BodyPart.h"

#include <vector>
#include <string>

Body::Body() { }

std::vector<BodyPart> &Body::getBodyParts() {

    return this->bodyparts;

}

Body &Body::operator+=(const BodyPart &bodyPart) {

    this->bodyparts.push_back(bodyPart);
    return *this;

}

std::string Body::to_string() {

    std::string result = "Body \n";

    for (std::vector<BodyPart>::iterator iter = this->getBodyParts().begin();
         iter != this->getBodyParts().end(); ++iter)
        result += (*iter).to_string() + "\n";

    return result;

}