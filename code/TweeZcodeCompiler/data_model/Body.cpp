//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "include/Passage/Body/BodyPart.h"

#include <vector>
#include <string>
#include <memory>

Body::Body() { }

std::vector<std::unique_ptr<BodyPart>> &Body::getBodyParts() {

    return this->bodyparts;

}

Body &Body::operator+=(const BodyPart &bodyPart) {

    this->bodyparts.push_back(std::unique_ptr<BodyPart>(&bodyPart));

    return *this;

}

std::string Body::to_string() {

    std::string result = "Body \n";

    for (std::vector<std::unique_ptr<BodyPart>>::iterator iter = this->getBodyParts().begin();
         iter != this->getBodyParts().end(); ++iter)
        result += (*iter)->to_string() + "\n";

    return result;

}