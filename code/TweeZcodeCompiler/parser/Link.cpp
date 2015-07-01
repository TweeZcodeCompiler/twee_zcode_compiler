//
// Created by lars on 05.06.15.
//

#include "include/Passage/Body/Link.h"

#include <string>

Link::Link(std::string target) {
    this->target = target;
    this->altName = target;
}

Link::Link(std::string target, std::string altName) {
    this->target = target;
    this->altName = altName;
}

std::string Link::getTarget() const {
    return this->target;
}

std::string Link::getAltName() const {
    return this->altName;
}

std::string Link::to_string() const {
    return std::string("Link \n")
           + std::string("      Target: ") + this->getTarget() + std::string("\n")
           + std::string("      AltName: ") + this->getAltName();
}