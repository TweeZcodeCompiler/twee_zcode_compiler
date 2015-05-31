//
// Created by lars on 31.05.15.
//

#include "include/PassageElements/Link.h"

#include <string>

Link::Link(Passage target) {

    this->target = target;

}

Link::Link(Passage target, std::string altName) {

    this->target = target;
    this->altName = altName;

}

std::string Link::to_string() {

    return "Link" + std::endl + "Name: " + this->altName + std::endl + "Target: " + this->target + std::endl;

}

std::string Link::to_ZASS() {

}