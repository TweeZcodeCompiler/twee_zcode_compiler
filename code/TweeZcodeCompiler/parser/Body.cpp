//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body.h"
#include "memory.h"
#include "include/Passage/Body/BodyPart.h"

using namespace std;

vector<unique_ptr<BodyPart>> &Body::getBodyParts() {
    return this->bodyparts;
}

Body &Body::operator+=(unique_ptr<BodyPart>& bodyPart) {
    this->bodyparts.push_back(move(bodyPart));
    return *this;
}

Body& Body::operator+=(BodyPart* bodyPart) {
    bodyparts.push_back(unique_ptr<BodyPart>(bodyPart));
    return *this;
}

std::string Body::to_string() {
    std::string result = "  Body \n";

    for (auto iter = bodyparts.begin(); iter != bodyparts.end(); ++iter)
        result += "    " + (*iter)->to_string() + "\n";

    return result;
}