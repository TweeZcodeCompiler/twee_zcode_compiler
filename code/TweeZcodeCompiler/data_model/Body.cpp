//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "include/Passage/Body/BodyPart.h"

#include <vector>
#include <string>

Body::Body() { }

vector <BodyPart> Body::getBodyParts() {

    return this->bodyparts;

}

Body &Body::operator+=(const BodyPart &bodyPart) {

    this->bodyparts.push_back(bodyPart);
    return *this;

}

std::string Body::to_string() {

    std::string result = "Body" + std::endl;

    for (int i = 0; i < passages.size(); ++i) {
        result += this->bodyparts[i].to_string() + std::endl;
    }

    return result;

}