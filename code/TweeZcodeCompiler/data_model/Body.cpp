//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "include/Passage/Body/BodyPart.h"

Body::Body() { }


Body::~Body() {
    for(auto it = bodyparts.begin(); it != bodyparts.end(); ++it) {
        delete *it;
    }
}

const std::vector<BodyPart*> &Body::getBodyParts() {
    return this->bodyparts;
}

Body &Body::operator+=(BodyPart* bodyPart) {
    this->bodyparts.push_back(bodyPart);
    return *this;
}

std::string Body::to_string() {

    std::string result = "Body \n";

    for (std::vector<BodyPart*>::iterator iter = bodyparts.begin();
         iter != this->getBodyParts().end(); ++iter)
        result += (*iter)->to_string() + "\n";

    return result;

}