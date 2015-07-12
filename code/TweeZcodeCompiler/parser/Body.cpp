//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "include/Passage/Body/BodyPart.h"

using namespace std;

const vector<unique_ptr<BodyPart>>& Body::getBodyParts() const {
    return bodyparts;
}

Body &Body::operator+=(BodyPart& bodyPart) {
    this->bodyparts.push_back(unique_ptr<BodyPart>(bodyPart.clone()));
    return *this;
}

std::string Body::to_string() const {
    std::string result = "  Body \n";

    for (auto iter = bodyparts.begin(); iter != bodyparts.end(); ++iter)
        result += "    " + (*iter)->to_string() + "\n";

    return result;
}
